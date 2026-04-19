#include "../include/KeyManager.hpp"

std::expected<std::array<uint8_t, KeyManager::FULL_KEY_SIZE>, Error> KeyManager::ReadKey( std::string_view inputKey ) {

    std::ifstream file( std::string(inputKey), std::ios::binary );

    if ( !file ) return std::unexpected( Error::NullInput );

    std::array<uint8_t, FULL_KEY_SIZE> buffer;

    if ( file.read( reinterpret_cast<char*>( buffer.data() ), FULL_KEY_SIZE ) ) {

        if ( file.peek() != EOF) return std::unexpected( Error::InvalidKeySize );

        return buffer;
    }

    return std::unexpected( Error::InvalidKeySize );
}


std::expected<void, Error> KeyManager::WriteKey( std::string_view outputKey, std::span<const uint8_t, FULL_KEY_SIZE> keyData ) {

    std::ofstream file( outputKey.data(), std::ios::binary );

    if ( !file ) return std::unexpected( Error::FileWriteError );

    if ( file.write( reinterpret_cast<const char*>( keyData.data() ), keyData.size() ) ) {

        return {};
    }

    return std::unexpected( Error::FileWriteError );
}


std::array<uint8_t, KeyManager::FULL_KEY_SIZE> KeyManager::GenerateKey() {

    std::array<uint8_t, FULL_KEY_SIZE> newKey;

    static std::random_device rd;
    static std::mt19937 gen( rd() );
    static std::uniform_int_distribution<int> dist( 0, 255 );

    for ( auto& byte : newKey ) byte = static_cast<uint8_t>( dist( gen ) );

    return newKey;
}