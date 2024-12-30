#include "../parser.h"

#include <iostream>

int main() {
    std::string url = "https://www.example.com";
    std::string htmlContent = R"(
        <html>
            <head>
                <title>Example Domain</title>
                <meta charset="UTF-8">
            </head>
            <body>
                <h1>Example Domain</h1>
                <p>This domain is for use in illustrative examples in documents.</p>
                <p><a href="https://www.iana.org/domains/example">More information...</a></p>
            </body>
        </html>
    )";

    ParsedData data = parseHTML(url, htmlContent);
    std::cout << "URL: " << data.first << std::endl;
    std::cout << "Number of Words Extracted: " << data.second.size() << std::endl;
    std::cout << "Extracted Words:" << std::endl;
    for (size_t i = 0; i < data.second.size(); ++i) {
        std::cout << i + 1 << ". " << data.second[i] << std::endl;
    }
    return 0;
}
