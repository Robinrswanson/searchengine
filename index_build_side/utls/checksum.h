#include <iostream>
#include <string>
#include <bitset>

uint8_t rabinFingerprint8Bit(const std::string& url) {
    // Define the irreducible polynomial (x^8 + x^4 + x^3 + x^2 + 1)
    uint16_t irreduciblePolynomial = 0b100011101; // Degree-8 polynomial
    
    // Convert URL to a binary representation
    uint64_t data = 0;
    for (char c : url) {
        data = (data << 8) | static_cast<uint8_t>(c); // Append ASCII values as binary
    }

    // Perform polynomial division
    while (data >= 256) { // While data is larger than 8 bits
        int shift = 8 * ((data >> 8) ? __builtin_clz(data >> 8) : 0) - 8; // Find leading bits beyond 8
        data ^= (irreduciblePolynomial << shift); // XOR with shifted polynomial
    }

    // The remainder is the 8-bit checksum
    return static_cast<uint8_t>(data);
}

int main() {
    std::string url = "https://example.com";
    uint8_t checksum = rabinFingerprint8Bit(url);
    
    // Print the checksum in binary format
    std::cout << "The 8-bit checksum for the URL is: " 
              << std::bitset<8>(checksum) 
              << " (decimal: " << static_cast<int>(checksum) << ")" 
              << std::endl;
    return 0;
}
