#include "../include/KeyManager.hpp"

std::expected<std::vector<uint8_t>, Error> KeyManager::ReadKey( std::string_view inputKey ) {

    std::ifstream file( std::string(inputKey), std::ios::binary );

    if ( !file ) return std::unexpected( Error::NullInput );

    std::vector<uint8_t> buffer;

    file.seekg( 0, std::ios::end );
    std::streamsize size = file.tellg();
    file.seekg( 0, std::ios::beg );

    if (size < 16) {
        return std::unexpected(Error::InvalidKeySize);
    }

    size_t bytesToRead = ( size > FULL_KEY_SIZE ) ? FULL_KEY_SIZE : static_cast<size_t>( size );
    buffer.resize( bytesToRead );


    if ( file.read( reinterpret_cast<char*>( buffer.data() ), bytesToRead ) ) {
        return buffer;
    }
    return std::unexpected( Error::InvalidKeySize );
}

std::expected<void, Error> KeyManager::WriteKey( std::string_view outputKey, std::span<const uint8_t> keyData ) {

    std::ofstream file( outputKey.data(), std::ios::binary );

    if ( !file ) return std::unexpected( Error::FileWriteError );

    if ( file.write( reinterpret_cast<const char*>( keyData.data() ), keyData.size() ) ) {

        return {};
    }

    return std::unexpected( Error::FileWriteError );
}

void KeyManager::AppendIVToKey( const std::string& path, const std::vector<uint8_t>& iv ) {

    std::ofstream ofs( path, std::ios::binary | std::ios::app ); 

    if (ofs) {
        ofs.write( reinterpret_cast<const char*>(iv.data()), iv.size() );
    }
}


std::array<uint8_t, Rabbit::KEY_SIZE> KeyManager::GenerateKey() {

    std::array<uint8_t, Rabbit::KEY_SIZE> newKey;

    static std::random_device rd;
    static std::mt19937 gen( rd() );
    static std::uniform_int_distribution<int> dist( 0, 255 );

    for ( auto& byte : newKey ) byte = static_cast<uint8_t>( dist( gen ) );

    return newKey;
}

std::array<uint8_t, Rabbit::IV_SIZE> KeyManager::GenerateIV() {

    std::array<uint8_t, Rabbit::IV_SIZE> newIV;

    static std::random_device rd;
    static std::mt19937 gen( rd() );
    static std::uniform_int_distribution<int> dist( 0, 255 );

    for (auto& byte : newIV) byte = static_cast<uint8_t>( dist( gen ) );

    return newIV;
}