#pragma once

#include <cstdint>
#include <cstddef>

class Rabbit {
private:
    struct RabbitState {
        uint32_t X[8];
        uint32_t C[8];
        uint32_t b;     // carry bit
    };

    RabbitState state;

    static constexpr uint32_t A[8] = { 0x4D34D34D, 0xD34D34D3,
        0x34D34D34, 0x4D34D34D, 0xD34D34D3,
        0x34D34D34, 0x4D34D34D, 0xD34D34D3 };

    uint32_t g( uint32_t u, uint32_t v );

    void Next();

public:
    static constexpr size_t KEY_SIZE = 16;
    static constexpr size_t IV_SIZE = 8;

    Rabbit();
    ~Rabbit();

    void Init( const uint8_t K[ KEY_SIZE ], const uint8_t IV[ IV_SIZE ] );

    void Strm( uint8_t Z[16] );

    void PrintState() const;
};