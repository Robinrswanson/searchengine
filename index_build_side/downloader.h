#include <iostream>
#include <string>
#include <fstream>

#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>

#define MAX_FILE_SIZE 256 * 1024
#define PACKET_SIZE 1024

std::string httpDownloader(std::string);
std::string httpsDownloader(std::string);

std::string getHostnameFromUrl(std::string);
std::string getHostPathFromUrl(std::string);

std::string httpDownloader(std::string url)
{
	int port = 80;

	std::string path = getHostPathFromUrl(url);
	std::string hostname = getHostnameFromUrl(url);

    struct addrinfo *address, hints;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo( hostname, port, &hints, &address ) != 0) {
        std::perror("getaddrinfo");
        return EXIT_FAILURE;
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(address);
        return EXIT_FAILURE;
    }

	if (connect(sockfd, address->ai_addr, sizeof(struct sockaddr)) != 0) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return EXIT_FAILURE;
	}

	std::string send_data = "GET " + path + " HTTP/1.1\r\nHOST:" + hostname + "\r\nConnection: Close\r\n\r\n";
	if (send(sockfd, send_data.c_str(), strlen(send_data.c_str()), 0) < 0) {
	    std::perror("send");
        close(sockfd);
        freeaddrinfo(address);
        return EXIT_FAILURE;
    }

	char buffer[PACKET_SIZE];
	std::string Response = "";

    int bytes_received;
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        Response += buffer;
    }

	close(sock);
    freeaddrinfo(address);

	return Response;
}

std::string getHostnameFromUrl(std::string url)
{
	int offset = 0;
	offset = offset == 0 && url.compare(0, 8, "https://") == 0 ? 8 : offset;
	offset = offset == 0 && url.compare(0, 7, "http://") == 0 ? 7 : offset;

	size_t pos = url.find("/", offset);
	std::string domain = url.substr(offset, (pos == std::string::npos ? url.length() : pos) - offset);

	return domain;
}
std::string getHostPathFromUrl(std::string url)
{
	int offset = 0;
	offset = offset == 0 && url.compare(0, 8, "https://") == 0 ? 8 : offset;
	offset = offset == 0 && url.compare(0, 7, "http://") == 0 ? 7 : offset;

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

std::string httpsDownloader(const std::string &url) {
    const int port = 443;
    std::string path = getHostPathFromUrl(url);
    std::string hostname = getHostnameFromUrl(url);

    // Resolve the address
    struct addrinfo hints{}, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &address) != 0) {
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

    // Initialize SSL
    SSL_CTX *ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        perror("SSL_CTX_new");
        close(sockfd);
        return "";
    }

    SSL *conn = SSL_new(ssl_ctx);
    if (!conn) {
        perror("SSL_new");
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    SSL_set_fd(conn, sockfd);
    if (SSL_connect(conn) <= 0) {
        perror("SSL_connect");
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    // Send GET request
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << hostname << "\r\n"
            << "Connection: close\r\n\r\n";

    if (SSL_write(conn, request.str().c_str(), request.str().length()) <= 0) {
        perror("SSL_write");
        SSL_shutdown(conn);
        SSL_free(conn);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return "";
    }

    // Read the response
    std::string response;
    char buffer[PACKET_SIZE];
    int bytesRead;
    while ((bytesRead = SSL_read(conn, buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytesRead);
    }

    // Cleanup
    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);
    close(sockfd);

    return response;
}

{
	int port = 443;
	std::string path = getHostPathFromUrl(url);
	std::string hostname = getHostnameFromUrl(url);

    struct addrinfo *address, hints;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo( hostname, port, &hints, &address ) != 0) {
        std::perror("getaddrinfo");
        return EXIT_FAILURE;
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(address);
        return EXIT_FAILURE;
    }

	if (connect(sockfd, address->ai_addr, sizeof(struct sockaddr)) != 0) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return EXIT_FAILURE;
	}

	// initialize OpenSSL - do this once and stash ssl_ctx in a global var
	SSL_load_error_std::strings();
	SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	SSL_library_init();

	// create an SSL connection and attach it to the socket
	SSL *conn = SSL_new(ssl_ctx);
	SSL_set_fd(conn, sockfd);

	// perform the SSL/TLS handshake with the server - when on the
	// server side, this would use SSL_accept()
	int err = SSL_connect(conn);
	if (err != 1)
	{
		// facing this error with https://aave.com/, some certification failing...
		return "SSL_connect() failed.";
	}

	// now proceed with HTTP traffic,
	//     using SSL_read instead of recv() and
	//     SSL_write instead of send(),
	std::string getRequest = "GET " + path + " HTTP/1.1\r\n" + "Host: " + hostname + "\r\n" +
											+"Connection: close\r\n\r\n";

	char tempBuff[10000];
	strcpy(tempBuff, getRequest.c_str());

	int writeRet = SSL_write(conn, tempBuff, getRequest.size());
	if (writeRet < 0)
	{
		return "Send Request Failed.";
	}

	std::string httpResponse = "";
	char ptr[PACKET_SIZE];
	int totalBytesRead = 0;
	int bytesRead;

	do
	{
		bytesRead = SSL_read(conn, ptr, sizeof(ptr) / sizeof(ptr[0]));
		totalBytesRead += bytesRead;
		httpResponse += std::string(ptr);
		if (httpResponse.size() > MAX_FILE_SIZE)
		{
			httpResponse = "";
			break;
		}
	} while (bytesRead);

	//and SSL_shutdown/SSL_free before close()
	SSL_shutdown(conn);
	SSL_free(conn);
	close(sock);


	return httpResponse;

}