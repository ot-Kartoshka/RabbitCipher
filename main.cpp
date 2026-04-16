
#include <iostream>
#include <iomanip>
#include "include/Rabbit.hpp" 

void PrintHex(const uint8_t* data, size_t size, const char* label) {
    std::cout << label << ": ";
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

int main() {
    Rabbit rabbit;
    uint8_t keystream[16];

    uint8_t key1[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    uint8_t iv1[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    rabbit.PrintState();
    rabbit.Init(key1, iv1);
    std::cout << "Стан ПІСЛЯ Init:\n";
    rabbit.PrintState();

    rabbit.Strm(keystream);
    PrintHex(keystream, 16, "");
    rabbit.PrintState();
    rabbit.Strm(keystream);
    PrintHex(keystream, 16,"");
    rabbit.PrintState();
    rabbit.Strm(keystream);
    PrintHex(keystream, 16, "");
    rabbit.PrintState();
    rabbit.Strm(keystream);
    PrintHex(keystream, 16, "");
    rabbit.PrintState();
    return 0;
}