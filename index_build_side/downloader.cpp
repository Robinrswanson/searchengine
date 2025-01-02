#include "downloader.h"

#include <iostream>
#include <fstream>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_FILE_SIZE 256 * 1024
#define BUFFER_SIZE 1024

std::string httpDownloader(const std::string& url)
{
    int port = 80;

    std::string path = getHostPathFromUrl(url);
    std::string hostname = getHostnameFromUrl(url);

    struct addrinfo hints, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Convert port number to string
    std::string portStr = std::to_string(port);

    if (getaddrinfo(hostname.c_str(), portStr.c_str(), &hints, &address) != 0) {
        std::perror("getaddrinfo");
        return "";
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(address);
        return "";
    }

    if (connect(sockfd, address->ai_addr, address->ai_addrlen) != 0) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return "";
    }

    freeaddrinfo(address);

    // Construct GET request with the correct path
    const char *http_get_start = "GET ";
    const char *http_get_end = " HTTP/1.1\r\nHost: ";
    const char *end_of_line = "\r\nConnection: close\r\n\r\n";
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    std::snprintf(buffer, sizeof(buffer), "%s%s%s%s", http_get_start, path.c_str(), http_get_end, hostname.c_str());

    // Append end_of_line
    strncat(buffer, end_of_line, sizeof(buffer) - strlen(buffer) - 1);

    if (send(sockfd, buffer, std::strlen(buffer), 0) == -1) {
        std::perror("send");
        close(sockfd);
        return std::to_string(EXIT_FAILURE);
    }

    std::string Response = "";

    int bytes_received;
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the buffer
        Response += buffer;
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (bytes_received == -1) {
        std::perror("recv");
    }
    close(sockfd);

    return Response;
}

std::string getHostnameFromUrl(const std::string& url)
{
    size_t offset = 0;
    if (url.compare(0, 8, "https://") == 0)
        offset = 8;
    else if (url.compare(0, 7, "http://") == 0)
        offset = 7;

    size_t pos = url.find("/", offset);
    std::string domain = url.substr(offset, (pos == std::string::npos ? url.length() : pos) - offset);

    return domain;
}

std::string getHostPathFromUrl(const std::string& url)
{
    size_t offset = 0;
    if (url.compare(0, 8, "https://") == 0)
        offset = 8;
    else if (url.compare(0, 7, "http://") == 0)
        offset = 7;

    size_t pos = url.find("/", offset);
    std::string path = pos == std::string::npos ? "/" : url.substr(pos);

    // Remove extra slashes
    pos = path.find_first_not_of('/');
    if (pos == std::string::npos)
        path = "/";
    else
        path.erase(0, pos - 1);
    return path;
}

std::string httpsDownloader(const std::string& url) {
    int port = 443;
    std::string path = getHostPathFromUrl(url);
    std::string hostname = getHostnameFromUrl(url);

    // Resolve the address
    struct addrinfo hints, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);

    if (getaddrinfo(hostname.c_str(), portStr.c_str(), &hints, &address) != 0) {
        perror("getaddrinfo");
        return "";
    }

    // Create socket
    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(address);
        return "";
    }

    // Connect to the server
    if (connect(sockfd, address->ai_addr, address->ai_addrlen) != 0) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return "";
    }

    freeaddrinfo(address);

    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Create SSL context
    SSL_CTX *ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        std::cerr << "SSL_CTX_new failed" << std::endl;
        close(sockfd);
        return "";
    }

    // Create SSL connection
    SSL *conn = SSL_new(ssl_ctx);
    if (!conn) {
        std::cerr << "SSL_new failed" << std::endl;
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    SSL_set_fd(conn, sockfd);
    if (SSL_connect(conn) <= 0) {
        std::cerr << "SSL_connect failed" << std::endl;
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    // Construct GET request with the correct path
    const char *http_get_start = "GET ";
    const char *http_get_end = " HTTP/1.1\r\nHost: ";
    const char *end_of_line = "\r\nConnection: close\r\n\r\n";
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    std::snprintf(buffer, sizeof(buffer), "%s%s%s%s", http_get_start, path.c_str(), http_get_end, hostname.c_str());

    // Append end_of_line
    strncat(buffer, end_of_line, sizeof(buffer) - strlen(buffer) - 1);

    if (SSL_write(conn, buffer, std::strlen(buffer)) <= 0) {
        std::cerr << "SSL_write failed" << std::endl;
        SSL_shutdown(conn);
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    // Read the response
    std::string response;
    int bytesRead;
    while ((bytesRead = SSL_read(conn, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        response.append(buffer, bytesRead);
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (bytesRead < 0) {
        std::cerr << "SSL_read failed" << std::endl;
    }

    // Cleanup
    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);
    close(sockfd);

    return response;
}
