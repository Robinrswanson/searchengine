#include "url.h"
#include <algorithm>
#include <iostream>
#include <cctype>


URL::URL() : port(0), path("/"), query(""), fragment("") {}

URL::URL(const std::string& url_str) : port(0), path("/"), query(""), fragment("") {
    parse(url_str);
}

void URL::parse(const std::string& url_str) {
    if (url_str.empty()) {
        std::cerr << "Error: URL string is empty." << std::endl;
        scheme = "";
        hostname = "";
        port = 0;
        path = "/";
        query = "";
        fragment = "";
        return;
    }

    size_t scheme_end = url_str.find("://");
    if (scheme_end == std::string::npos) {
        std::cerr << "Error: Invalid URL format (missing '://')." << std::endl;
        scheme = "";
        hostname = "";
        port = 0;
        path = "/";
        query = "";
        fragment = "";
        return;
    }

    scheme = url_str.substr(0, scheme_end);
    std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);

    size_t host_start = scheme_end + 3;
    size_t host_end = url_str.find_first_of("/?#", host_start);
    if (host_end == std::string::npos) {
        host_end = url_str.length();
    }

    hostname = url_str.substr(host_start, host_end - host_start);
    std::transform(hostname.begin(), hostname.end(), hostname.begin(), ::tolower);

    size_t port_pos = hostname.find(':');
    if (port_pos != std::string::npos) {
        std::string port_str = hostname.substr(port_pos + 1);
        hostname = hostname.substr(0, port_pos);
        try {
            port = std::stoi(port_str);
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid port number '" << port_str << "'. Assigning default port." << std::endl;
            if (scheme == "https") {
                port = 443;
            } else if (scheme == "http") {
                port = 80;
            } else {
                port = 0;
            }
        }
    } else {
        if (scheme == "https") {
            port = 443;
        } else if (scheme == "http") {
            port = 80;
        } else {
            std::cerr << "Error: Unsupported scheme '" << scheme << "'." << std::endl;
            scheme = "";
            hostname = "";
            port = 0;
            path = "/";
            query = "";
            fragment = "";
            return;
        }
    }

    path = "/";
    query = "";
    fragment = "";

    size_t current_pos = host_end;
    while (current_pos < url_str.length()) {
        char current_char = url_str[current_pos];
        if (current_char == '/') {
            size_t path_start = current_pos;
            size_t path_end = url_str.find_first_of("?#", path_start);
            if (path_end == std::string::npos) {
                path_end = url_str.length();
            }
            path = url_str.substr(path_start, path_end - path_start);
            current_pos = path_end;
        } else if (current_char == '?') {
            size_t query_start = current_pos + 1;
            size_t query_end = url_str.find('#', query_start);
            if (query_end == std::string::npos) {
                query_end = url_str.length();
            }
            query = url_str.substr(query_start, query_end - query_start);
            current_pos = query_end;
        } else if (current_char == '#') {
            size_t fragment_start = current_pos + 1;
            fragment = url_str.substr(fragment_start);
            break;
        } else {
            std::cerr << "Error: Unexpected character '" << current_char << "' in URL." << std::endl;
            break;
        }
    }

    if (path.empty() || path[0] != '/')
        path = "/" + path;
}

std::string URL::toString() const {
    std::string url = scheme + "://" + hostname;

    if ((scheme == "http" && port != 80) ||
        (scheme == "https" && port != 443)) {
        url += ":" + std::to_string(port);
    }

    url += path;

    if (!query.empty()) {
        url += "?" + query;
    }

    if (!fragment.empty()) {
        url += "#" + fragment;
    }

    return url;
}

bool URL::isValid() const {
    return !scheme.empty() && !hostname.empty();
}
