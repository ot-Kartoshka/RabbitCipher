#pragma once

#include <cstdint>
#include <cstddef>
#include <span>


class Rabbit {

    public:

        static constexpr size_t KEY_SIZE = 16;
        static constexpr size_t IV_SIZE = 8;
        static constexpr size_t Z_SIZE = 16;

        Rabbit() noexcept;
        ~Rabbit();

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

        uint32_t g( uint32_t u, uint32_t v ) const noexcept;

        void Next() noexcept;

        void Init(std::span<const uint8_t, KEY_SIZE> K, std::span<const uint8_t, IV_SIZE> IV) noexcept;

        void Strm(std::span<uint8_t, Rabbit::Z_SIZE> Z) noexcept;

        void XorData(std::span< const uint8_t > input, std::span<uint8_t> output) noexcept;


        friend class RabbitManager;

};