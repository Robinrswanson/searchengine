#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

struct ParsedData {
    std::string url;                     // The URL of the parsed page
    std::vector<std::string> words;      // Words extracted from the page
    std::vector<std::string> links;      // Links found on the page
};

ParsedData parseHTML(const std::string& url, const std::string& html);

#endif
