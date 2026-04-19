#pragma once

#include <fstream>
#include <expected>
#include <array>
#include <random>

#include "Rabbit.hpp"
#include "Errors.hpp"

class KeyManager {

    public:

        static constexpr size_t FULL_KEY_SIZE = Rabbit::KEY_SIZE + Rabbit::IV_SIZE;

        static std::expected<std::array<uint8_t, FULL_KEY_SIZE>, Error> ReadKey( std::string_view inputKey );

        static std::expected<void, Error> WriteKey( std::string_view outputKey, std::span<const uint8_t, FULL_KEY_SIZE> keyData );

        static std::array<uint8_t, FULL_KEY_SIZE> GenerateKey();
};