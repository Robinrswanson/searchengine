#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

// Struct to hold the HTTP response header details
struct ResponseHeader {
    int status = 0; // HTTP status code (e.g., 200, 404, 301)
    std::string contentType; // Content-Type of the response (e.g., text/html)
    int contentLength = -1; // Content-Length of the response body (-1 if not provided)
};

// Struct to represent the entire HTTP/HTTPS response
struct Response {
    ResponseHeader header; // Parsed response header
    std::string body;      // Response body content
};

ResponseHeader handle_response(const std::string& response);

// Functions for downloading content
Response httpDownloader(std::string& url);
Response httpsDownloader(std::string& url);

// Utility functions for parsing URLs
std::string getHostnameFromUrl(const std::string& url);
std::string getHostPathFromUrl(const std::string& url);

#endif // DOWNLOADER_H
