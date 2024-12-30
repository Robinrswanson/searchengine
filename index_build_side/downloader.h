#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

std::string httpDownloader(const std::string& url);
std::string httpsDownloader(const std::string& url);

std::string getHostnameFromUrl(const std::string& url);
std::string getHostPathFromUrl(const std::string& url);

#endif