#include <iostream>
#include <cassert>
#include "../downloader.h"

// Utility function to check if a string contains valid HTML structure
bool isValidHtml(const std::string& content) {
    return content.find("<!DOCTYPE html>") != std::string::npos ||
           (content.find("<html>") != std::string::npos && content.find("</html>") != std::string::npos);
}

// Test Case 1: Validate HTTP downloader returns valid HTML
void testHttpDownloader() {
    std::string url = "http://example.com"; // Ensure this URL is reachable
    std::string content = httpDownloader(url);

    assert(!content.empty() && "HTTP downloader should return non-empty content");
    assert(isValidHtml(content) && "HTTP downloader should return valid HTML content");

    std::cout << "Test Case 1: HTTP downloader passed.\n";
}

// Test Case 2: Validate HTTPS downloader returns valid HTML
void testHttpsDownloader() {
    std::string url = "https://www.example.com"; // Ensure this URL is reachable
    std::string content = httpsDownloader(url);

    assert(!content.empty() && "HTTPS downloader should return non-empty content");
    assert(isValidHtml(content) && "HTTPS downloader should return valid HTML content");

    std::cout << "Test Case 2: HTTPS downloader passed.\n";
}

// Test Case 3: Handle unreachable HTTP URL
void testUnreachableUrlHttp() {
    std::string url = "http://thisdomaindoesnotexist.xyz";

    try {
        std::string content = httpDownloader(url);
        assert(content.empty() && "HTTP downloader should return empty content for unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable HTTP URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 3: Unreachable HTTP URL passed.\n";
}

// Test Case 4: Handle unreachable HTTPS URL
void testUnreachableUrlHttps() {
    std::string url = "https://thisdomaindoesnotexist.xyz";

    try {
        std::string content = httpsDownloader(url);
        assert(content.empty() && "HTTPS downloader should return empty content for unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable HTTPS URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 4: Unreachable HTTPS URL passed.\n";
}

// Main function to run all test cases
int main() {
    try {
        testHttpDownloader();
        testHttpsDownloader();
        testUnreachableUrlHttp();
        testUnreachableUrlHttps();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }

    std::cout << "All test cases passed successfully.\n";
    return 0;
}
