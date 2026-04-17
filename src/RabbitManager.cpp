#include "../include/RabbitManager.hpp"

RabbitManager::RabbitManager() noexcept {

}

RabbitManager::~RabbitManager() {

}

std::vector<uint8_t> RabbitManager::DecryptData( std::span<const uint8_t> cipher, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept {

	std::vector<uint8_t> plain( cipher.size() );
	rabbit.XorData( cipher, K, IV, plain );
	return plain;
}

std::vector<uint8_t> RabbitManager::EncryptData( std::span<const uint8_t> plain, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept {

	std::vector<uint8_t> cipher( plain.size() );
	rabbit.XorData( plain, K, IV, cipher );
	return cipher;
}

