#pragma once

#include <expected>
#include <vector>
#include <sstream>
#include <print>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "Rabbit.hpp"
#include "Errors.hpp"
#include "KeyManager.hpp"


class RabbitManager {

    private:


        Rabbit rabbit;

        bool ask_yes_no(std::string_view message, bool auto_yes);
        int ask_choice(std::string_view message, int default_choice, bool auto_yes);

        std::string bytes_to_hex(std::span<const uint8_t> data);
        std::vector<uint8_t> hex_to_bytes(const std::string& hex);

        std::expected<void, Error> ProcessStream(Rabbit& cipher, std::ifstream& in, std::ofstream& out);

    public:
        struct Options {
            bool encrypt_mode = true;
            bool generate_key = false;
            bool no_iv = false;
            bool save_iv_to_key = false;
            std::string force_iv_from = "";
            bool yes_to_all = false;
        };

        static constexpr size_t BUFFER_SIZE = 4 * 1024 * 1024;

        RabbitManager() noexcept;
        ~RabbitManager();

        std::vector<uint8_t> EncryptData( std::span<const uint8_t> plain, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept;
        std::vector<uint8_t> DecryptData( std::span<const uint8_t> cipher, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept;

        std::expected<void, Error> Conductor( std::string_view inputPath, std::string_view outputPath, std::string_view keyPath, const Options& opt );

};