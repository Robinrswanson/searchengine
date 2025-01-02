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

#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib> // for std::stoi

ResponseHeader handle_response(const std::string& response) {
    ResponseHeader header;
    std::istringstream response_stream(response);
    std::string line;

    while (std::getline(response_stream, line) && line != "\r") {
        if (line.find("HTTP/") == 0) {
            // Extract status code from the status line
            size_t status_start = line.find(" ") + 1; // First space
            if (status_start != std::string::npos) {
                size_t status_end = line.find(" ", status_start); // Second space
                if (status_end != std::string::npos) {
                    try {
                        header.status = std::stoi(line.substr(status_start, status_end - status_start));
                    } catch (const std::exception& e) {
                        header.status = 0; // Default to 0 if conversion fails
                    }
                }
            }
        } else if (line.find("Content-Type:") == 0) {
            header.contentType = line.substr(13); // Extract value after "Content-Type:"
        } else if (line.find("Content-Length:") == 0) {
            std::string length_str = line.substr(15); // Extract value after "Content-Length:"
            try {
                header.contentLength = std::stoi(length_str);
            } catch (const std::exception& e) {
                header.contentLength = -1; // Default to -1 if conversion fails
            }
        }
    }

    // Trim whitespace from contentType
    auto trim = [](std::string& str) {
        str.erase(0, str.find_first_not_of(" \t\r\n"));
        str.erase(str.find_last_not_of(" \t\r\n") + 1);
    };

    trim(header.contentType);

    return header;
}

Response httpDownloader(std::string& url) {
    int port = 80;

    std::string path = getHostPathFromUrl(url);
    std::string hostname = getHostnameFromUrl(url);

    struct addrinfo hints{}, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);

    if (getaddrinfo(hostname.c_str(), portStr.c_str(), &hints, &address) != 0) {
        std::perror("getaddrinfo");
        return {};
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(address);
        return {};
    }

    if (connect(sockfd, address->ai_addr, address->ai_addrlen) != 0) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return {};
    }

    freeaddrinfo(address);

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";
    if (send(sockfd, request.c_str(), request.size(), 0) == -1) {
        std::perror("send");
        close(sockfd);
        return {};
    }

    Response response;
    std::string response_header;
    char buffer[BUFFER_SIZE];
    bool header_parsed = false;

    while (int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) {
        if (bytes_received < 0) {
            std::perror("recv");
            break;
        }

        buffer[bytes_received] = '\0';
        if (!header_parsed) {
            std::string chunk(buffer);
            size_t header_end = chunk.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                response_header += chunk.substr(0, header_end + 4);
                response.body += chunk.substr(header_end + 4);
                header_parsed = true;

                response.header = handle_response(response_header);
            } else {
                response_header += chunk;
            }
        } else {
            response.body += buffer;
        }
    }

    close(sockfd);
    return response;
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

Response httpsDownloader(std::string& url) {
    int port = 443;
    std::string path = getHostPathFromUrl(url);
    std::string hostname = getHostnameFromUrl(url);

    struct addrinfo hints{}, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);

    if (getaddrinfo(hostname.c_str(), portStr.c_str(), &hints, &address) != 0) {
        perror("getaddrinfo");
        return {};
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(address);
        return {};
    }

    if (connect(sockfd, address->ai_addr, address->ai_addrlen) != 0) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return {};
    }

    freeaddrinfo(address);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        std::cerr << "SSL_CTX_new failed" << std::endl;
        close(sockfd);
        return {};
    }

    SSL* conn = SSL_new(ssl_ctx);
    if (!conn) {
        std::cerr << "SSL_new failed" << std::endl;
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return {};
    }

    SSL_set_fd(conn, sockfd);
    if (SSL_connect(conn) <= 0) {
        std::cerr << "SSL_connect failed" << std::endl;
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return {};
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";
    if (SSL_write(conn, request.c_str(), request.size()) <= 0) {
        std::cerr << "SSL_write failed" << std::endl;
        SSL_shutdown(conn);
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return {};
    }

    Response response;
    std::string response_header;
    char buffer[BUFFER_SIZE];
    bool header_parsed = false;

    while (int bytes_read = SSL_read(conn, buffer, sizeof(buffer) - 1)) {
        if (bytes_read < 0) {
            std::cerr << "SSL_read failed" << std::endl;
            break;
        }

        buffer[bytes_read] = '\0';
        if (!header_parsed) {
            std::string chunk(buffer);
            size_t header_end = chunk.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                response_header += chunk.substr(0, header_end + 4);
                response.body += chunk.substr(header_end + 4);
                header_parsed = true;

                response.header = handle_response(response_header);
            } else {
                response_header += chunk;
            }
        } else {
            response.body += buffer;
        }
    }

    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);
    close(sockfd);
    return response;
}