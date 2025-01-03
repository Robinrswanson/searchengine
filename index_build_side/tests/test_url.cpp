// tests/test_url.cpp

#include <iostream>
#include <cassert>
#include "../include/url.h" // Corrected include path

// Helper function to print test case headers
void printTestHeader(const std::string& testName) {
    std::cout << "=== " << testName << " ===" << std::endl;
}

// Test Case 1: Valid HTTP URL Parsing
void testValidHttpUrl() {
    printTestHeader("Test Case 1: Valid HTTP URL Parsing");
    std::string url_str = "http://www.example.com/path?query=123#fragment";
    URL url(url_str);
    if (url.isValid() &&
        url.scheme == "http" &&
        url.hostname == "example.com" &&
        url.port == 80 &&
        url.path == "/path" &&
        url.query == "query=123" &&
        url.fragment == "fragment" &&
        url.toString() == "http://example.com/path?query=123#fragment") {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed.\n\n";
    }
}

// Test Case 2: Valid HTTPS URL Parsing with Specified Port
void testValidHttpsUrlWithPort() {
    printTestHeader("Test Case 2: Valid HTTPS URL Parsing with Specified Port");
    std::string url_str = "https://www.example.com:8443/secure?token=abc#section";
    URL url(url_str);
    if (url.isValid() &&
        url.scheme == "https" &&
        url.hostname == "example.com" &&
        url.port == 8443 &&
        url.path == "/secure" &&
        url.query == "token=abc" &&
        url.fragment == "section" &&
        url.toString() == "https://example.com:8443/secure?token=abc#section") {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed.\n\n";
    }
}

// Test Case 3: Valid HTTPS URL Parsing without Port
void testValidHttpsUrlNoPort() {
    printTestHeader("Test Case 3: Valid HTTPS URL Parsing without Port");
    std::string url_str = "https://www.example.com/secure?token=abc#section";
    URL url(url_str);
    if (url.isValid() &&
        url.scheme == "https" &&
        url.hostname == "example.com" &&
        url.port == 443 &&
        url.path == "/secure" &&
        url.query == "token=abc" &&
        url.fragment == "section" &&
        url.toString() == "https://example.com/secure?token=abc#section") {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed.\n\n";
    }
}

// Test Case 4: Invalid URL Parsing (Missing Scheme)
void testInvalidUrlMissingScheme() {
    printTestHeader("Test Case 4: Invalid URL Parsing (Missing Scheme)");
    std::string url_str = "://example.com";
    URL url(url_str);
    if (!url.isValid()) {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed: URL should be invalid due to missing scheme.\n\n";
    }
}

// Test Case 5: Invalid URL Parsing (Unsupported Scheme)
void testInvalidUrlUnsupportedScheme() {
    printTestHeader("Test Case 5: Invalid URL Parsing (Unsupported Scheme)");
    std::string url_str = "ftp://www.example.com/resource";
    URL url(url_str);
    if (!url.isValid()) {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed: URL should be invalid due to unsupported scheme.\n\n";
    }
}

// Test Case 6: Invalid URL Parsing (Empty String)
void testInvalidUrlEmptyString() {
    printTestHeader("Test Case 6: Invalid URL Parsing (Empty String)");
    std::string url_str = "";
    URL url(url_str);
    if (!url.isValid()) {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed: URL should be invalid due to empty string.\n\n";
    }
}

// Test Case 7: Valid HTTP URL without 'www.'
void testValidHttpUrlNoWww() {
    printTestHeader("Test Case 7: Valid HTTP URL without 'www.'");
    std::string url_str = "http://example.com/about";
    URL url(url_str);
    if (url.isValid() &&
        url.scheme == "http" &&
        url.hostname == "example.com" &&
        url.port == 80 &&
        url.path == "/about" &&
        url.query.empty() &&
        url.fragment.empty() &&
        url.toString() == "http://example.com/about") {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed.\n\n";
    }
}

// Test Case 8: Valid HTTP URL with Query and Fragment
void testValidHttpUrlWithQueryFragment() {
    printTestHeader("Test Case 8: Valid HTTP URL with Query and Fragment");
    std::string url_str = "http://www.example.com/search?q=cpp#results";
    URL url(url_str);
    if (url.isValid() &&
        url.scheme == "http" &&
        url.hostname == "example.com" &&
        url.port == 80 &&
        url.path == "/search" &&
        url.query == "q=cpp" &&
        url.fragment == "results" &&
        url.toString() == "http://example.com/search?q=cpp#results") {
        std::cout << "Passed.\n\n";
    } else {
        std::cerr << "Failed.\n\n";
    }
}

// Main function to run all test cases
int main() {
    testValidHttpUrl();
    testValidHttpsUrlWithPort();
    testValidHttpsUrlNoPort();
    testInvalidUrlMissingScheme();
    testInvalidUrlUnsupportedScheme();
    testInvalidUrlEmptyString();
    testValidHttpUrlNoWww();
    testValidHttpUrlWithQueryFragment();

    std::cout << "All URL test cases executed.\n";
    return 0;
}
