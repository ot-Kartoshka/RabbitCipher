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

        static std::expected<std::vector<uint8_t>, Error> ReadKey( std::string_view inputKey );

        static std::expected<void, Error> WriteKey( std::string_view outputKey, std::span<const uint8_t> keyData );

        static void AppendIVToKey( const std::string& path, const std::vector<uint8_t>& iv );

        static std::array<uint8_t, Rabbit::KEY_SIZE> GenerateKey();
        static std::array<uint8_t, Rabbit::IV_SIZE> GenerateIV();
};