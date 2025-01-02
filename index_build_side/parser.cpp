#include "parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

// Helper function to convert a string to lowercase
std::string toLower(const std::string& str) {
    std::string lowerStr;
    lowerStr.reserve(str.size());
    for (char ch : str) {
        lowerStr += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return lowerStr;
}

std::string handleAnchorTag(const std::string& baseUrl, const std::string& tagContent) {
    std::regex hrefRegex(R"(href\s*=\s*["']([^"']+)["'])");
    std::smatch match;

    if (std::regex_search(tagContent, match, hrefRegex)) {
        std::string href = match[1].str();

        // If the href is already an absolute URL, return it
        if (href.find("http://") == 0 || href.find("https://") == 0) {
            return href;
        }

        // Convert relative URL to absolute URL
        std::string base = baseUrl;
        if (base.back() != '/') {
            base += '/';
        }

        if (href.front() == '/') {
            // Remove trailing slash from base URL if href starts with '/'
            base = base.substr(0, base.find_last_of('/') + 1);
        }

        return base + href;
    }

    // Return an empty string if no href is found
    return "";
}

std::string stripHTMLTags(const std::string& url, const std::string& html, std::vector<std::string>& links) {
    std::string result;
    bool insideTag = false;
    std::string tagBuffer;
    result.reserve(html.size());

    for (char ch : html) {
        if (ch == '<') {
            insideTag = true;
            tagBuffer.clear();
            continue;
        }
        if (ch == '>') {
            insideTag = false;

            // Process the full tag content
            if (!tagBuffer.empty()) {
                // Check if it's an anchor tag
                if (tagBuffer.find("a ") == 0 || tagBuffer == "a") {
                    std::string fullAnchor = "<" + tagBuffer + ">";
                    std::string absoluteUrl = handleAnchorTag(url, fullAnchor);
                    if (!absoluteUrl.empty()) {
                        links.push_back(absoluteUrl);
                    }
                }
            }
            continue;
        }
        if (insideTag) {
            tagBuffer += ch;
        } else {
            result += ch; // Append non-tag content to the result
        }
    }
    return result;
}

std::vector<std::string> splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string currentWord;
    currentWord.reserve(16);

    for (char ch : text) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            currentWord += ch;
        } else {
            if (!currentWord.empty()) {
                words.push_back(toLower(currentWord));
                currentWord.clear();
            }
        }
    }

    if (!currentWord.empty()) {
        words.push_back(toLower(currentWord));
    }

    return words;
}

// The main parser function
ParsedData parseHTML(const std::string& url, const std::string& html) {
    std::vector<std::string> links;
    
    // Step 1: Strip HTML tags and collect links
    std::string text = stripHTMLTags(url, html, links);

    // Step 2: Split text into words
    std::vector<std::string> extractedWords = splitIntoWords(text);

    return {url, extractedWords, links};
}
