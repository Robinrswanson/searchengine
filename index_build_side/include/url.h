// include/url.h

#ifndef URL_H
#define URL_H

#include <string>

struct URL {
    std::string scheme;      // e.g., "http", "https"
    std::string hostname;    // e.g., "example.com"
    int port;                // e.g., 80, 443
    std::string path;        // e.g., "/about"
    std::string query;       // e.g., "id=123"
    std::string fragment;    // e.g., "section1"

    // Default constructor
    URL();

    // Constructor that parses a URL string
    explicit URL(const std::string& url_str);

    // Utility method to convert URL components back to a string
    std::string toString() const;

    // Utility method to check if the URL is valid
    bool isValid() const;

private:
    // Helper method to parse the URL
    void parse(const std::string& url_str);
};

#endif // URL_H
