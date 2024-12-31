#include <iostream>
#include <fstream>
#include <cassert>
#include "downloader.h"

// Utility function to save downloaded content to a file for verification
void saveToFile(const std::string& content, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write to file: " + filePath);
    }
    file << content;
    file.close();
}

// Test Case 1: Download a valid HTML file using HTTP
void testHttpDownloader() {
    std::string url = "http://example.com"; // Ensure this URL is reachable
    std::string outputFile = "http_example.html";

    std::string content = httpDownloader(url);
    assert(!content.empty() && "HTTP downloader should return valid HTML content");

    saveToFile(content, outputFile);

    std::cout << "Test Case 1: HTTP downloader passed.\n";
}

// Test Case 2: Download a valid HTML file using HTTPS
void testHttpsDownloader() {
    std::string url = "https://www.example.com"; // Ensure this URL is reachable
    std::string outputFile = "https_example.html";

    std::string content = httpsDownloader(url);
    assert(!content.empty() && "HTTPS downloader should return valid HTML content");

    saveToFile(content, outputFile);

    std::cout << "Test Case 2: HTTPS downloader passed.\n";
}

// Test Case 3: Parse hostname from URL
void testGetHostnameFromUrl() {
    std::string url = "https://www.example.com/path/to/resource";
    std::string expectedHostname = "www.example.com";

    std::string hostname = getHostnameFromUrl(url);
    assert(hostname == expectedHostname && "Hostname should match expected value");

    std::cout << "Test Case 3: Hostname parsing passed.\n";
}

// Test Case 4: Parse host path from URL
void testGetHostPathFromUrl() {
    std::string url = "https://www.example.com/path/to/resource";
    std::string expectedHostPath = "/path/to/resource";

    std::string hostPath = getHostPathFromUrl(url);
    assert(hostPath == expectedHostPath && "Host path should match expected value");

    std::cout << "Test Case 4: Host path parsing passed.\n";
}

// Test Case 5: Handle unreachable URL with HTTP downloader
void testUnreachableUrlHttp() {
    std::string url = "http://thisdomaindoesnotexist.xyz";

    try {
        std::string content = httpDownloader(url);
        assert(content.empty() && "HTTP downloader should fail for an unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 5: Unreachable URL with HTTP passed.\n";
}

// Test Case 6: Handle unreachable URL with HTTPS downloader
void testUnreachableUrlHttps() {
    std::string url = "https://thisdomaindoesnotexist.xyz";

    try {
        std::string content = httpsDownloader(url);
        assert(content.empty() && "HTTPS downloader should fail for an unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 6: Unreachable URL with HTTPS passed.\n";
}

// Main function to run all test cases
int main() {
    try {
        testHttpDownloader();
        testHttpsDownloader();
        testGetHostnameFromUrl();
        testGetHostPathFromUrl();
        testUnreachableUrlHttp();
        testUnreachableUrlHttps();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }

    std::cout << "All test cases passed successfully.\n";
    return 0;
}
