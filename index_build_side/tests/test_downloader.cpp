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
    Response response = httpDownloader(url);

    assert(response.header.status == 200 && "HTTP downloader should return a 200 OK status");
    assert(!response.body.empty() && "HTTP downloader should return non-empty body content");
    assert(isValidHtml(response.body) && "HTTP downloader should return valid HTML content");

    std::cout << "Test Case 1: HTTP downloader passed.\n";
}

// Test Case 2: Validate HTTPS downloader returns valid HTML
void testHttpsDownloader() {
    std::string url = "https://www.example.com"; // Ensure this URL is reachable
    Response response = httpsDownloader(url);

    assert(response.header.status == 200 && "HTTPS downloader should return a 200 OK status");
    assert(!response.body.empty() && "HTTPS downloader should return non-empty body content");
    assert(isValidHtml(response.body) && "HTTPS downloader should return valid HTML content");

    std::cout << "Test Case 2: HTTPS downloader passed.\n";
}

// Test Case 3: Handle unreachable HTTP URL
void testUnreachableUrlHttp() {
    std::string url = "http://thisdomaindoesnotexist.xyz";

    try {
        Response response = httpDownloader(url);
        assert(response.header.status != 200 && "HTTP downloader should not return a 200 status for unreachable URL");
        assert(response.body.empty() && "HTTP downloader should return empty body for unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable HTTP URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 3: Unreachable HTTP URL passed.\n";
}

// Test Case 4: Handle unreachable HTTPS URL
void testUnreachableUrlHttps() {
    std::string url = "https://thisdomaindoesnotexist.xyz";

    try {
        Response response = httpsDownloader(url);
        assert(response.header.status != 200 && "HTTPS downloader should not return a 200 status for unreachable URL");
        assert(response.body.empty() && "HTTPS downloader should return empty body for unreachable URL");
    } catch (const std::exception& e) {
        std::cout << "Handled unreachable HTTPS URL exception: " << e.what() << '\n';
    }

    std::cout << "Test Case 4: Unreachable HTTPS URL passed.\n";
}

// Test Case 5: Handle redirect URL for HTTP
void testRedirectUrlHTTP() {
    std::string url = "http://httpbin.org/redirect/1"; // Replace with a URL that redirects

    Response response = httpDownloader(url);

    assert(response.header.status >= 300 && response.header.status < 400 &&
           "Redirect URL should return a 3xx status code");
    assert(!response.header.contentType.empty() && "Redirect URL response should include headers");

    std::cout << "Test Case 5: Redirect URL passed.\n";
}

// Test Case 6: Handle redirect URL for HTTPS
void testRedirectUrlHTTPS() {
    std::string url = "https://httpbin.org/redirect/1"; // Replace with a URL that redirects

    Response response = httpsDownloader(url);

    assert(response.header.status >= 300 && response.header.status < 400 &&
           "Redirect URL should return a 3xx status code");
    assert(!response.header.contentType.empty() && "Redirect URL response should include headers");

    std::cout << "Test Case 6: Redirect URL passed.\n";
}

// Main function to run all test cases
int main() {
    try {
        testHttpDownloader();
        testHttpsDownloader();
        testUnreachableUrlHttp();
        testUnreachableUrlHttps();
        testRedirectUrlHTTP();
        testRedirectUrlHTTPS();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }

    std::cout << "All test cases passed successfully.\n";
    return 0;
}
