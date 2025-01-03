// src/crawler.cpp

#include <iostream>
#include <queue>
#include <unordered_set>
#include <string>
#include <regex>
#include "downloader.h"
#include "parser.h"
#include "url.h"

// Function to process a single URL: download, parse, and extract links
void processUrl(const URL& url, std::queue<URL>& urlQueue, std::unordered_set<std::string>& visitedUrls, int& redirectCount, int maxRedirects = 5) {
    std::cout << "Crawling: " << url.toString() << std::endl;

    // Download the page
    Response response = downloadPage(url);

    // Check if the response is a redirect (3xx)
    if (response.header.status >= 300 && response.header.status < 400) {
        if (!response.header.location.empty()) {
            if (redirectCount >= maxRedirects) {
                std::cerr << "Max redirects reached for URL: " << url.toString() << std::endl;
                return;
            }

            std::cout << "Redirect (" << response.header.status << ") from " << url.toString() 
                      << " to " << response.header.location << std::endl;

            // Parse the new URL
            try {
                URL redirectUrl(response.header.location);

                // If the redirect URL is relative, construct the absolute URL based on the current URL
                if (redirectUrl.scheme.empty()) {
                    // Assume same scheme and hostname
                    std::string newUrl = url.scheme + "://" + url.hostname + redirectUrl.path;
                    if (!redirectUrl.query.empty()) {
                        newUrl += "?" + redirectUrl.query;
                    }
                    if (!redirectUrl.fragment.empty()) {
                        newUrl += "#" + redirectUrl.fragment;
                    }
                    redirectUrl = URL(newUrl);
                }

                if (redirectUrl.isValid() && visitedUrls.find(redirectUrl.toString()) == visitedUrls.end()) {
                    urlQueue.push(redirectUrl);
                    visitedUrls.insert(redirectUrl.toString());
                    redirectCount++;
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid redirect URL: " << response.header.location 
                          << " (" << e.what() << ")" << std::endl;
            }
        }
        return;
    }

    // Check if the download was successful
    if (response.header.status != 200) {
        std::cerr << "Failed to fetch URL: " << url.toString() 
                  << " (HTTP " << response.header.status << ")" << std::endl;
        return;
    }

    // Parse the HTML content
    ParsedData parsedData = parseHTML(url.toString(), response.body);

    // Output extracted words
    std::cout << "Extracted Words:" << std::endl;
    for (const std::string& word : parsedData.words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    // Enqueue extracted links
    for (const std::string& link : parsedData.links) {
        try {
            URL extractedUrl(link);

            // If the extracted URL is relative, construct the absolute URL based on the current URL
            if (extractedUrl.scheme.empty()) {
                // Handle relative URL
                std::string newUrl = url.scheme + "://" + url.hostname + extractedUrl.path;
                if (!extractedUrl.query.empty()) {
                    newUrl += "?" + extractedUrl.query;
                }
                if (!extractedUrl.fragment.empty()) {
                    newUrl += "#" + extractedUrl.fragment;
                }
                extractedUrl = URL(newUrl);
            }

            if (extractedUrl.isValid() && visitedUrls.find(extractedUrl.toString()) == visitedUrls.end()) {
                urlQueue.push(extractedUrl);
                visitedUrls.insert(extractedUrl.toString());
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid extracted URL: " << link << " (" << e.what() << ")" << std::endl;
            continue;
        }
    }
}

// Function to crawl URLs starting from the initial URL
void crawl(const URL& startUrl, int maxUrls = 1000, int maxRedirects = 5) {
    std::queue<URL> urlQueue;
    std::unordered_set<std::string> visitedUrls;
    int crawlCount = 0;

    // Start with the initial URL
    urlQueue.push(startUrl);
    visitedUrls.insert(startUrl.toString());

    while (!urlQueue.empty() && crawlCount < maxUrls) {
        URL currentUrl = urlQueue.front();
        urlQueue.pop();

        int redirectCount = 0;
        processUrl(currentUrl, urlQueue, visitedUrls, redirectCount, maxRedirects);

        crawlCount++;
    }

    if (crawlCount >= maxUrls) {
        std::cout << "Reached maximum crawl limit of " << maxUrls << " URLs." << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        URL startUrl(argv[1]);

        if (!startUrl.isValid()) {
            std::cerr << "Invalid start URL: " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }

        crawl(startUrl);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error parsing URL: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
