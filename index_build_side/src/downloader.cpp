// src/downloader.cpp

#include "downloader.h"
#include "url.h"
#include <iostream>
#include <fstream>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <algorithm>
#include <memory>

// Constants
constexpr int MAX_FILE_SIZE = 256 * 1024;
constexpr int BUFFER_SIZE = 1024;

// Helper function to trim whitespace from a string
void trim(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
}

// Struct to hold HTTP response headers
// Defined in downloader.h

// Function to handle HTTP response headers
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
        } else if (line.find("Location:") == 0) {
            header.location = line.substr(10); // Extract value after "Location:"
        }
    }

    // Trim whitespace from contentType and location
    trim(header.contentType);
    trim(header.location);

    return header;
}

// RAII wrapper for SSL_CTX
struct SSL_CTX_Deleter {
    void operator()(SSL_CTX* ctx) const {
        if (ctx) SSL_CTX_free(ctx);
    }
};

using SSL_CTX_ptr = std::unique_ptr<SSL_CTX, SSL_CTX_Deleter>;

// RAII wrapper for SSL
struct SSL_Deleter {
    void operator()(SSL* ssl) const {
        if (ssl) SSL_free(ssl);
    }
};

using SSL_ptr = std::unique_ptr<SSL, SSL_Deleter>;

// Function to create and connect a socket
int create_and_connect_socket(const std::string& hostname, const std::string& port) {
    struct addrinfo hints{}, *address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP; // TCP

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &address) != 0) {
        std::perror("getaddrinfo");
        return -1;
    }

    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(address);
        return -1;
    }

    if (connect(sockfd, address->ai_addr, address->ai_addrlen) != 0) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(address);
        return -1;
    }

    freeaddrinfo(address);
    return sockfd;
}

// Function to send HTTP GET request over plain socket or SSL
bool send_request(int sockfd, const std::string& request, SSL* ssl = nullptr) {
    size_t total_sent = 0;
    size_t to_send = request.size();
    const char* data = request.c_str();

    while (total_sent < to_send) {
        ssize_t sent;
        if (ssl) {
            sent = SSL_write(ssl, data + total_sent, to_send - total_sent);
            if (sent <= 0) {
                int err = SSL_get_error(ssl, sent);
                std::cerr << "SSL_write failed with error code: " << err << std::endl;
                return false;
            }
        } else {
            sent = send(sockfd, data + total_sent, to_send - total_sent, 0);
            if (sent == -1) {
                std::perror("send");
                return false;
            }
        }
        total_sent += sent;
    }
    return true;
}

// Function to read HTTP response from plain socket or SSL
bool read_response(int sockfd, bool is_ssl, SSL* ssl, Response& response) {
    char buffer[BUFFER_SIZE];
    bool header_parsed = false;
    std::string response_header;

    while (true) {
        ssize_t bytes_read;
        if (is_ssl) {
            bytes_read = SSL_read(ssl, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                int err = SSL_get_error(ssl, bytes_read);
                if (err == SSL_ERROR_ZERO_RETURN) {
                    break; // Connection closed cleanly
                } else {
                    std::cerr << "SSL_read failed with error code: " << err << std::endl;
                    return false;
                }
            }
        } else {
            bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
            if (bytes_read <= 0)
                break;
        }

        if (!header_parsed) {
            // Search for header-body separator
            std::string chunk(buffer, bytes_read);
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
            response.body.append(buffer, bytes_read);
        }

        // Optional: Limit response size to prevent excessive memory usage
        if (response.body.size() > MAX_FILE_SIZE) {
            std::cerr << "Exceeded maximum file size limit." << std::endl;
            return false;
        }
    }

    return true;
}

// Function to initialize OpenSSL (once)
void initialize_openssl() {
    static bool initialized = false;
    if (!initialized) {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        initialized = true;
    }
}

// Function to perform HTTPS download
Response httpsDownloader(const URL& url) {
    int port = url.port; // 443 if not specified
    std::string path = url.path;
    std::string hostname = url.hostname;

    int sockfd = create_and_connect_socket(hostname, std::to_string(port));
    if (sockfd == -1) {
        return {};
    }

    initialize_openssl();

    SSL_CTX_ptr ssl_ctx(SSL_CTX_new(TLS_client_method()));
    if (!ssl_ctx) {
        std::cerr << "SSL_CTX_new failed" << std::endl;
        close(sockfd);
        return {};
    }

    SSL_ptr ssl(SSL_new(ssl_ctx.get()));
    if (!ssl) {
        std::cerr << "SSL_new failed" << std::endl;
        close(sockfd);
        return {};
    }

    if (SSL_set_fd(ssl.get(), sockfd) == 0) {
        std::cerr << "SSL_set_fd failed" << std::endl;
        close(sockfd);
        return {};
    }

    if (SSL_connect(ssl.get()) <= 0) {
        std::cerr << "SSL_connect failed" << std::endl;
        close(sockfd);
        return {};
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";
    if (!send_request(sockfd, request, ssl.get())) {
        SSL_shutdown(ssl.get());
        return {};
    }

    Response response;
    if (!read_response(sockfd, true, ssl.get(), response)) {
        SSL_shutdown(ssl.get());
        return {};
    }

    SSL_shutdown(ssl.get());
    close(sockfd);

    return response;
}

// Function to perform HTTP download
Response httpDownloader(const URL& url) {
    int port = url.port; // 80 if not specified
    std::string path = url.path;
    std::string hostname = url.hostname;

    int sockfd = create_and_connect_socket(hostname, std::to_string(port));
    if (sockfd == -1) {
        return {};
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";
    if (!send_request(sockfd, request)) {
        close(sockfd);
        return {};
    }

    Response response;
    if (!read_response(sockfd, false, nullptr, response)) {
        close(sockfd);
        return {};
    }

    close(sockfd);
    return response;
}

// Function to download a page based on the URL scheme
Response downloadPage(const URL& url) {
    if (url.scheme == "https") {
        return httpsDownloader(url);
    } else if (url.scheme == "http") {
        return httpDownloader(url);
    } else {
        std::cerr << "Unsupported URL scheme: " << url.scheme << std::endl;
        return {};
    }
}
