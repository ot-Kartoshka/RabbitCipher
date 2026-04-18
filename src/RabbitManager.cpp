#include "../include/RabbitManager.hpp"

RabbitManager::RabbitManager() noexcept {

}

RabbitManager::~RabbitManager() {

}

std::vector<uint8_t> RabbitManager::DecryptData( std::span<const uint8_t> cipher, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept {

	std::vector<uint8_t> plain( cipher.size() );

	rabbit.Init(K, IV);
	rabbit.XorData( cipher, plain );

	return plain;
}

std::vector<uint8_t> RabbitManager::EncryptData( std::span<const uint8_t> plain, std::span<const uint8_t, Rabbit::KEY_SIZE> K, std::span<const uint8_t, Rabbit::IV_SIZE> IV ) noexcept {

	std::vector<uint8_t> cipher( plain.size() );

	rabbit.Init(K, IV);
	rabbit.XorData( plain, cipher );
	
	return cipher;
}

std::expected<void, Error> RabbitManager::Execute( std::string_view inputPath, std::string_view outputPath, std::string_view keyPath, bool isEncryption ) {

	auto KeyData = FileHandler::ReadFile( keyPath );

	if ( !KeyData ) {
		return std::unexpected( KeyData.error() );
	}
	if ( KeyData->size() != Rabbit::KEY_SIZE + Rabbit::IV_SIZE ) {
		return std::unexpected( Error::InvalidKeySize );
	}

	std::span<const uint8_t, Rabbit::KEY_SIZE> K( KeyData->data(), Rabbit::KEY_SIZE );
	std::span<const uint8_t, Rabbit::IV_SIZE> IV( KeyData->data() + Rabbit::KEY_SIZE, Rabbit::IV_SIZE );

	std::ifstream inputFile(inputPath.data(), std::ios::binary);

	if ( !inputFile ) {
		return std::unexpected( Error::NullInput );
	}

	std::ofstream outputFile(outputPath.data(), std::ios::binary);

	if ( !outputFile ) {
		return std::unexpected(Error::FileWriteError);
	}

	rabbit.Init( K, IV );

	constexpr size_t BUFFER_SIZE = 4 * 1024 * 1024 ;

	std::vector<uint8_t> buffer(BUFFER_SIZE);

	while ( inputFile ) {

		inputFile.read( reinterpret_cast<char*>( buffer.data() ), BUFFER_SIZE );
		std::streamsize bytesRead = inputFile.gcount();

		if ( bytesRead > 0 ) {
			std::span<const uint8_t> dataSpan( buffer.data(), static_cast<size_t>( bytesRead ) );
			rabbit.XorData( dataSpan, dataSpan );
			outputFile.write( reinterpret_cast<const char*>( dataSpan.data() ), dataSpan.size() );
		}
	}
	
	return {};
}