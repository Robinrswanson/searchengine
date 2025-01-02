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

    // Output URL
    std::cout << "URL: " << data.url << std::endl;

    // Output Words
    std::cout << "Number of Words Extracted: " << data.words.size() << std::endl;
    std::cout << "Extracted Words:" << std::endl;
    for (size_t i = 0; i < data.words.size(); ++i) {
        std::cout << i + 1 << ". " << data.words[i] << std::endl;
    }

    // Output Links
    std::cout << "Number of Links Extracted: " << data.links.size() << std::endl;
    std::cout << "Extracted Links:" << std::endl;
    for (size_t i = 0; i < data.links.size(); ++i) {
        std::cout << i + 1 << ". " << data.links[i] << std::endl;
    }

    return 0;
}
