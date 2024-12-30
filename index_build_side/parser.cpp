// parser.cpp
#include "parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>

// Helper function to convert a string to lowercase
std::string toLower(const std::string& str) {
    std::string lowerStr;
    lowerStr.reserve(str.size());
    for (char ch : str) {
        lowerStr += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return lowerStr;
}

// TODO: change different tags as < and > could very well be used inefficiently
std::string stripHTMLTags(const std::string& html) {
    std::string result;
    bool insideTag = false;
    result.reserve(html.size());

    for (char ch : html) {
        if (ch == '<') {
            insideTag = true;
            continue;
        }
        if (ch == '>') {
            insideTag = false;
            continue;
        }
        if (!insideTag) {
            result += ch;
        }
    }
    return result;
}

std::vector<std::string> splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string currentWord;
    currentWord.reserve(16); // Reserve space to minimize reallocations

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

    // Add the last word if exists
    if (!currentWord.empty()) {
        words.push_back(toLower(currentWord));
    }

    return words;
}

// The main parser function
ParsedData parseHTML(const std::string& url, const std::string& html) {
    // Step 1: Strip HTML tags to get raw text
    std::string text = stripHTMLTags(html);

    // Step 2: Split text into words
    std::vector<std::string> extractedWords = splitIntoWords(text);

    return {url, extractedWords};
}
