#include <iostream>
#include <iomanip> // For std::hex, std::setw, std::setfill

void printBufferAsHex(const unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;
}

int main() {
    unsigned char data[] = {0x01, 0x0A, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x90};
    printBufferAsHex(data, sizeof(data));
    return 0;
}