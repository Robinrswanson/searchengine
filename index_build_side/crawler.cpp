#include <iostream>
#include <queue>
#include <unordered_set>
#include <string>
#include <regex>
#include "downloader.h"
#include "parser.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string startUrl = argv[1];
    std::queue<std::string> urlQueue;
    std::unordered_set<std::string> visitedUrls;

    // Start with the initial URL
    urlQueue.push(startUrl);
    visitedUrls.insert(startUrl);

    while (!urlQueue.empty()) {
        std::string currentUrl = urlQueue.front();
        urlQueue.pop();

        std::cout << "Crawling: " << currentUrl << std::endl;

        // Download the page
        Response response;
        if (currentUrl.find("https://") == 0) {
            response = httpsDownloader(currentUrl);
        } else if (currentUrl.find("http://") == 0) {
            response = httpDownloader(currentUrl);
        } else {
            std::cerr << "Invalid URL scheme: " << currentUrl << std::endl;
            continue;
        }

        // Check if the download was successful
        if (response.header.status != 200) {
            std::cerr << "Failed to fetch URL: " << currentUrl << " (HTTP " << response.header.status << ")" << std::endl;
            continue;
        }

        // Parse the HTML content
        ParsedData parsedData = parseHTML(currentUrl, response.body);

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

    return EXIT_SUCCESS;
}
