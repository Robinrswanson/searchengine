// tests/test_downloader.cpp

#include <iostream>
#include <cassert>
#include "../include/downloader.h"
#include "../include/url.h" // Include the URL struct

// Helper function to print test case headers
void printTestHeader(const std::string& testName) {
    std::cout << "=== " << testName << " ===" << std::endl;
}

// Utility function to check if a string contains valid HTML structure
bool isValidHtml(const std::string& content) {
    return content.find("<!DOCTYPE html>") != std::string::npos ||
           (content.find("<html>") != std::string::npos && content.find("</html>") != std::string::npos);
}

// Test Case 1: Validate HTTP downloader returns valid HTML
void testHttpDownloader() {
    printTestHeader("Test Case 1: Validate HTTP Downloader Returns Valid HTML");
    try {
        std::string url_str = "http://example.com"; // Ensure this URL is reachable
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");
        // Perform download
        Response response = httpDownloader(url);

        // Assertions
        assert(response.header.status == 200 && "HTTP downloader should return a 200 OK status");
        assert(!response.body.empty() && "HTTP downloader should return non-empty body content");
        assert(isValidHtml(response.body) && "HTTP downloader should return valid HTML content");

        // Optional: Verify specific content (update as needed)
        // std::string expected_snippet = "<h1>Example Domain</h1>";
        // assert(response.body.find(expected_snippet) != std::string::npos && "Content mismatch");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Test Case 2: Validate HTTPS downloader returns valid HTML
void testHttpsDownloader() {
    printTestHeader("Test Case 2: Validate HTTPS Downloader Returns Valid HTML");
    try {
        std::string url_str = "https://www.example.com"; // Ensure this URL is reachable
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");

        // Perform download
        Response response = httpsDownloader(url);

        // Assertions
        assert(response.header.status == 200 && "HTTPS downloader should return a 200 OK status");
        assert(!response.body.empty() && "HTTPS downloader should return non-empty body content");
        assert(isValidHtml(response.body) && "HTTPS downloader should return valid HTML content");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Test Case 3: Handle unreachable HTTP URL
void testUnreachableUrlHttp() {
    printTestHeader("Test Case 3: Handle Unreachable HTTP URL");
    try {
        std::string url_str = "http://thisdomaindoesnotexist.xyz"; // Unreachable URL
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");

        // Perform download
        Response response = httpDownloader(url);

        // Assertions based on downloader implementation
        assert(response.header.status != 200 && "HTTP downloader should not return a 200 status for unreachable URL");
        assert(response.body.empty() && "HTTP downloader should return empty body for unreachable URL");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Test Case 4: Handle unreachable HTTPS URL
void testUnreachableUrlHttps() {
    printTestHeader("Test Case 4: Handle Unreachable HTTPS URL");
    try {
        std::string url_str = "https://thisdomaindoesnotexist.xyz"; // Unreachable URL
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");

        // Perform download
        Response response = httpsDownloader(url);

        // Assertions based on downloader implementation
        assert(response.header.status != 200 && "HTTPS downloader should not return a 200 status for unreachable URL");
        assert(response.body.empty() && "HTTPS downloader should return empty body for unreachable URL");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Test Case 5: Handle redirect URL for HTTP
void testRedirectUrlHTTP() {
    printTestHeader("Test Case 5: Handle Redirect URL for HTTP");
    try {
        std::string url_str = "http://httpbin.org/redirect/1"; // URL that redirects
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");

        // Perform download
        Response response = httpDownloader(url);

        // Assertions
        assert(response.header.status >= 300 && response.header.status < 400 &&
               "Redirect URL should return a 3xx status code");
        assert(!response.header.location.empty() && "Redirect URL response should include Location header");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Test Case 6: Handle redirect URL for HTTPS
void testRedirectUrlHTTPS() {
    printTestHeader("Test Case 6: Handle Redirect URL for HTTPS");
    try {
        std::string url_str = "https://httpbin.org/redirect/1"; // URL that redirects
        URL url(url_str);
        assert(url.isValid() && "URL should be valid");

        // Perform download
        Response response = httpsDownloader(url);

        // Assertions
        assert(response.header.status >= 300 && response.header.status < 400 &&
               "Redirect URL should return a 3xx status code");
        assert(!response.header.location.empty() && "Redirect URL response should include Location header");

        std::cout << "Passed.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << "\n\n";
    }
}

// Main function to run all test cases
int main() {
    testHttpDownloader();
    testHttpsDownloader();
    testUnreachableUrlHttp();
    testUnreachableUrlHttps();
    testRedirectUrlHTTP();
    testRedirectUrlHTTPS();

    std::cout << "All Downloader test cases executed.\n";
    return 0;
}
