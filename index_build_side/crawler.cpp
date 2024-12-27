#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <regex>

std::pair<std::string, std::string> parse_url(const std::string& url) {
    // Regex to match a URL and extract the hostname and port
    std::regex url_regex("^(http://|https://)?([^:/]+)(?::(\\d+))?");
    std::smatch matches;

    if (std::regex_match(url, matches, url_regex)) {
        std::string hostname = matches[2].str();
        std::string port = (matches[3].str().empty()) ? "80" : matches[3].str(); // Default to 80 if no port is specified
        return std::make_pair(hostname, port);
    } else {
        throw std::invalid_argument("Invalid URL format");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse URL
    std::string url = argv[1];
    std::string hostname;
    std::string port;
    try {
        std::tie(hostname, port) = parse_url(url);
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Hostname: " << hostname << ", Port: " << port << std::endl;

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP stream socket
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    // Get host address
    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res) != 0) {
        std::perror("getaddrinfo");
        return EXIT_FAILURE;
    }

    // Create socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        std::perror("socket");
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    // Connect socket to host address
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    // Send GET request
    const char *http_get = "GET / HTTP/1.1\r\nHost: ";
    const char *end_of_line = "\r\nConnection: close\r\n\r\n";
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), "%s%s%s", http_get, hostname.c_str(), end_of_line);
    std::cout << "Sending GET request: " << buffer << std::endl;

    if (send(sockfd, buffer, std::strlen(buffer), 0) == -1) {
        std::perror("send");
        close(sockfd);
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    // Read from socket and print to stdout until EOF
    int bytes_received;
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << buffer;
    }

    if (bytes_received == -1) {
        std::perror("recv");
    }

    // Close socket and free address
    close(sockfd);
    freeaddrinfo(res);

    return 0;
}
