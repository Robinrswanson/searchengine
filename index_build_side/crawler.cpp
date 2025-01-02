#include <iostream>
#include <queue>
#include <unordered_set>
#include <string>
#include <regex>
#include "downloader.h"
#include "parser.h"

// Function to process a single URL: download, parse, and extract links
void processUrl(std::string& url, std::queue<std::string>& urlQueue, std::unordered_set<std::string>& visitedUrls) {
    std::cout << "Crawling: " << url << std::endl;

    // Download the page
    Response response = downloadPage(url);

    // Check if the download was successful
    if (response.header.status != 200) {
        std::cerr << "Failed to fetch URL: " << url << " (HTTP " << response.header.status << ")" << std::endl;
        return;
    }

    // Parse the HTML content
    ParsedData parsedData = parseHTML(url, response.body);

    // Output extracted words
    std::cout << "Extracted Words:" << std::endl;
    for (const std::string& word : parsedData.words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    // Enqueue extracted links
    for (const std::string& link : parsedData.links) {
        if (visitedUrls.find(link) == visitedUrls.end()) {
            urlQueue.push(link);
            visitedUrls.insert(link);
        }
    }
}

// Function to crawl URLs starting from the initial URL
void crawl(const std::string& startUrl) {
    std::queue<std::string> urlQueue;
    std::unordered_set<std::string> visitedUrls;

    // Start with the initial URL
    urlQueue.push(startUrl);
    visitedUrls.insert(startUrl);

    while (!urlQueue.empty()) {
        std::string currentUrl = urlQueue.front();
        urlQueue.pop();
        processUrl(currentUrl, urlQueue, visitedUrls);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string startUrl = argv[1];
    crawl(startUrl);

    return EXIT_SUCCESS;
}
