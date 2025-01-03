#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>
#include "url.h" 

struct ResponseHeader {
    int status = 0;                 // HTTP status code (e.g., 200, 404, 301)
    std::string contentType;        // Content-Type of the response (e.g., text/html)
    int contentLength = -1;         // Content-Length of the response body (-1 if not provided)
    std::string location;           // Redirect location if status is 3xx
};

struct Response {
    ResponseHeader header; // Parsed response header
    std::string body;      // Response body content
};

// Function declarations
Response downloadPage(const URL& url);
Response httpsDownloader(const URL& url);
Response httpDownloader(const URL& url);

#endif // DOWNLOADER_H
