#include <bit>     
#include <ranges>    
#include <algorithm>

#include "../include/Rabbit.hpp"

Rabbit::Rabbit() noexcept {

	std::ranges::fill( state.X, 0 );
	std::ranges::fill( state.C, 0 );
	state.b = 0;

}

Rabbit::~Rabbit() {

	std::ranges::fill( state.X, 0 );
	std::ranges::fill( state.C, 0 );
	state.b = 0;

}

uint32_t Rabbit::g( uint32_t u, uint32_t v ) const noexcept {

	uint32_t sum = u + v;
	uint64_t temp = static_cast<uint64_t>( sum ) * sum;

	return static_cast<uint32_t>( temp ^ ( temp >> 32 ) );
}

void Rabbit::Next() noexcept {
    
	for ( int j = 0; j < 8; ++j ) {
		uint64_t temp = static_cast<uint64_t>( state.C[j] ) + A[j] + state.b;
		state.b = static_cast<uint32_t>( temp >> 32 ); 
		state.C[j] = static_cast<uint32_t>( temp );
	}

	uint32_t G[8];

	for ( int j = 0; j < 8; ++j ) {
		G[j] = g( state.X[j], state.C[j] );
	}

	state.X[0] = G[0] + std::rotl( G[7], 16 ) + std::rotl( G[6], 16 );
	state.X[1] = G[1] + std::rotl( G[0], 8 ) + G[7];
	state.X[2] = G[2] + std::rotl( G[1], 16 ) + std::rotl( G[0], 16 );
	state.X[3] = G[3] + std::rotl( G[2], 8 ) + G[1];
	state.X[4] = G[4] + std::rotl( G[3], 16 ) + std::rotl( G[2], 16 );
	state.X[5] = G[5] + std::rotl( G[4], 8 ) + G[3];
	state.X[6] = G[6] + std::rotl( G[5], 16 ) + std::rotl( G[4], 16 );
	state.X[7] = G[7] + std::rotl( G[6], 8 ) + G[5];

}

void Rabbit::Init( std::span<const uint8_t, KEY_SIZE> K, std::span<const uint8_t, IV_SIZE> IV ) noexcept {

	uint16_t K_[8];
	for ( int i = 0; i < 8; ++i ) {
		K_[i] = static_cast<uint16_t>( K[ i * 2 + 1 ] << 8 ) | K[ i * 2 ];
	}

	state.b = 0;
	for ( int j = 0; j < 8 ; ++j ) {
		
		if ( j % 2 == 0 ) {
			state.X[j] = ( static_cast<uint32_t>( K_[ (j + 1) % 8 ] ) << 16 ) | K_[j];
			state.C[j] = ( static_cast<uint32_t>( K_[ (j + 4) % 8 ] ) << 16 ) | K_[ (j + 5) % 8 ];
		} 
		else {
			state.X[j] = ( static_cast<uint32_t>( K_[ (j + 5) % 8 ] ) << 16 ) | K_[ (j + 4) % 8 ];
			state.C[j] = ( static_cast<uint32_t>( K_[j] ) << 16 ) | K_[ (j + 1) % 8 ];
		}

	}

	for ( int i = 0; i < 4; ++i ) {
		Next();
	}
	
	uint32_t IV_[2];

	IV_[0] = static_cast<uint32_t>( IV[0] ) | ( static_cast<uint32_t>( IV[1] ) << 8 ) | ( static_cast<uint32_t>( IV[2] ) << 16 ) | ( static_cast<uint32_t>( IV[3] ) << 24 );
	IV_[1] = static_cast<uint32_t>( IV[4] ) | ( static_cast<uint32_t>( IV[5] ) << 8 ) | ( static_cast<uint32_t>( IV[6] ) << 16 ) | ( static_cast<uint32_t>( IV[7] ) << 24 );

	state.C[0] ^= state.X[4] ^ IV_[0];
	state.C[1] ^= state.X[5] ^ ( ( IV_[1] & 0xFFFF0000 ) | ( IV_[0] >> 16 ) );
	state.C[2] ^= state.X[6] ^ IV_[1];
	state.C[3] ^= state.X[7] ^ ( ( IV_[1] << 16 ) | ( IV_[0] & 0x0000FFFF ) );
	state.C[4] ^= state.X[0] ^ IV_[0];
	state.C[5] ^= state.X[1] ^ ( ( IV_[1] & 0xFFFF0000 ) | ( IV_[0] >> 16 ) );
	state.C[6] ^= state.X[2] ^ IV_[1];
	state.C[7] ^= state.X[3] ^ ( ( IV_[1] << 16 ) | ( IV_[0] & 0x0000FFFF ) );

	for ( int i = 0; i < 4; ++i ) {
		Next();
	}

}

void Rabbit::Strm( std::span<uint8_t, Z_SIZE> Z ) noexcept {
	
	Next();

	uint16_t Z_[8];

	Z_[0] = static_cast<uint16_t>( state.X[0] ) ^ static_cast<uint16_t>( state.X[5] >> 16 );
	Z_[1] = static_cast<uint16_t>( state.X[0] >> 16 ) ^ static_cast<uint16_t>( state.X[3] );
	Z_[2] = static_cast<uint16_t>( state.X[2] ) ^ static_cast<uint16_t>( state.X[7] >> 16 );
	Z_[3] = static_cast<uint16_t>( state.X[2] >> 16 ) ^ static_cast<uint16_t>( state.X[5] );
	Z_[4] = static_cast<uint16_t>( state.X[4] ) ^ static_cast<uint16_t>( state.X[1] >> 16 );
	Z_[5] = static_cast<uint16_t>( state.X[4] >> 16 ) ^ static_cast<uint16_t>( state.X[7] );
	Z_[6] = static_cast<uint16_t>( state.X[6] ) ^ static_cast<uint16_t>( state.X[3] >> 16 );
	Z_[7] = static_cast<uint16_t>( state.X[6] >> 16 ) ^ static_cast<uint16_t>( state.X[1] );

	for (int i = 0; i < 8; ++i) {
		Z[ 2 * i ] = static_cast<uint8_t>( Z_[i] & 0xFF );
		Z[ 2 * i + 1 ] = static_cast<uint8_t>( ( Z_[i] >> 8 ) & 0xFF );
	}

}

void Rabbit::XorData( std::span< const uint8_t > input, std::span<uint8_t> output ) noexcept {

	std::array<uint8_t, Z_SIZE> keystream;

	for ( size_t i = 0; i < input.size(); ++i ) {

		if ( i % Z_SIZE == 0 ) {
			Strm( keystream );
		}

		output[i] = input[i] ^ keystream[ i % Z_SIZE ];
	}

}