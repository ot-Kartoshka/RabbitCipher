#pragma once

#include <string_view>
#include <expected>
#include <vector>
#include <new>


#include "Rabbit.hpp"
#include "Errors.hpp"
#include "FileHandler.hpp"


class RabbitManager {
private:


    Rabbit rabbit;


public:
    RabbitManager() noexcept;
    ~RabbitManager();

    std::vector<uint8_t> EncryptData(std::span<const uint8_t> plain, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV) noexcept;
    std::vector<uint8_t> DecryptData(std::span<const uint8_t> cipher, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV) noexcept;

    std::expected<void, Error> Conductor(std::string_view inputPath, std::string_view outputPath, std::string_view keyPath, bool isEncryption);

};