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

std::expected<void, Error> RabbitManager::Conductor( std::string_view inputPath, std::string_view outputPath, std::string_view keyPath, bool KeyGen ) {

	std::array< uint8_t, KeyManager::FULL_KEY_SIZE > KeyData;

	if ( KeyGen ) {

		KeyData = KeyManager::GenerateKey();
		auto writeKeyData = KeyManager::WriteKey( keyPath, KeyData );
		if ( !writeKeyData ) return std::unexpected( writeKeyData.error() );

	}
	else {

		auto readKey = KeyManager::ReadKey( keyPath );
		if ( !readKey ) return std::unexpected( readKey.error() );

		KeyData = readKey.value();
	
	}

	std::span<const uint8_t, Rabbit::KEY_SIZE> K( KeyData.data(), Rabbit::KEY_SIZE );
	std::span<const uint8_t, Rabbit::IV_SIZE> IV( KeyData.data() + Rabbit::KEY_SIZE, Rabbit::IV_SIZE );

	std::ifstream inputFile( inputPath.data(), std::ios::binary );

	if ( !inputFile ) return std::unexpected( Error::NullInput );

	std::ofstream outputFile( outputPath.data(), std::ios::binary );

	if ( !outputFile ) return std::unexpected( Error::FileWriteError );

	rabbit.Init( K, IV );

	std::vector<uint8_t> buffer( BUFFER_SIZE );

	while ( inputFile.read(reinterpret_cast<char*>(buffer.data()), BUFFER_SIZE) || inputFile.gcount() > 0 ) {

		std::streamsize bytesRead = inputFile.gcount();

		if (bytesRead > 0) {

			std::span<uint8_t> dataSpan( buffer.data(), static_cast<size_t>( bytesRead ) );
			rabbit.XorData( dataSpan, dataSpan );
			outputFile.write( reinterpret_cast<const char*>( dataSpan.data() ), dataSpan.size() );

			if ( !outputFile ) return std::unexpected( Error::FileWriteError );
		}
	}

	inputFile.close();
	outputFile.close();

	return {};
}