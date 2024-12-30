#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

using ParsedData = std::pair<std::string, std::vector<std::string>>;

ParsedData parseHTML(const std::string& url, const std::string& html);

#endif
