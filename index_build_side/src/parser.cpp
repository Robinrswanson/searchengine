#include "../include/parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

// Helper function to decode common HTML entities
std::string decodeHTMLEntities(const std::string& text) {
    std::string decoded;
    decoded.reserve(text.size());
    size_t i = 0;
    while (i < text.size()) {
        if (text[i] == '&') {
            if (text.compare(i, 4, "&lt;") == 0) {
                decoded += '<';
                i += 4;
            }
            else if (text.compare(i, 4, "&gt;") == 0) {
                decoded += '>';
                i += 4;
            }
            else if (text.compare(i, 5, "&amp;") == 0) {
                decoded += '&';
                i += 5;
            }
            // Add more entities as needed
            else {
                decoded += text[i];
                i++;
            }
        }
        else {
            decoded += text[i];
            i++;
        }
    }
    return decoded;
}


// Helper function to convert a string to lowercase
std::string toLower(const std::string& str) {
    std::string lowerStr;
    lowerStr.reserve(str.size());
    for (char ch : str) {
        lowerStr += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return lowerStr;
}

std::string getHostname(const std::string& url) {
    // Extract the hostname from the URL
    std::regex urlRegex(R"(^(https?://)?([^/]+))");
    std::smatch match;
    if (std::regex_search(url, match, urlRegex)) {
        return match[2].str(); // Return the hostname
    }
    return ""; // Return empty string if no match
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

        // Extract the hostname from the base URL
        std::string hostname = getHostname(baseUrl);
        if (hostname.empty()) {
            return ""; // Return empty string if hostname extraction fails
        }

        // Convert relative URL to absolute URL
        if (href.front() == '/') {
            return "https://" + hostname + href;
        } else {
            return "https://" + hostname + '/' + href;
        }
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

    // Decode HTML entities in the resultant text
    result = decodeHTMLEntities(result);
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
