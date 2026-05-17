	#include "../include/RabbitManager.hpp"

	namespace fs = std::filesystem;

	RabbitManager::RabbitManager() noexcept {

	}

	RabbitManager::~RabbitManager() {

	}

	bool RabbitManager::ask_yes_no( std::string_view message, bool auto_yes ) {	

		if ( auto_yes ) return true;

		std::print( "{} (y/N): ", message );
		std::string input;
		std::getline( std::cin, input );

		return !input.empty() && (input[0] == 'y' || input[0] == 'Y' || input[0] == 'т' || input[0] == 'Т');
	}

	int RabbitManager::ask_choice( std::string_view message, int default_choice, bool auto_yes ) {

		if ( auto_yes ) return default_choice;

		std::print( "{}: ", message );
		std::string input;
		std::getline( std::cin, input );

		if ( input.empty() ) return default_choice;

		std::stringstream ss( input );
		int value;

		if ( ss >> value ) return value;

		return default_choice;
	}

	std::string RabbitManager::bytes_to_hex( std::span<const uint8_t> data ) {

		std::string hex;
		hex.reserve( data.size() * 2 );
		constexpr char hex_chars[] = "0123456789ABCDEF";
		for ( uint8_t b : data ) {

			hex.push_back( hex_chars[b >> 4] );
			hex.push_back( hex_chars[b & 0x0F] );

		}
		return hex;
	}

	std::vector<uint8_t> RabbitManager::hex_to_bytes(const std::string& hex) {

		std::vector<uint8_t> bytes;

		if (hex.length() % 2 != 0) return bytes;

		bytes.reserve(hex.length() / 2);
		for (size_t i = 0; i < hex.length(); i += 2) {

			uint8_t byte;
			auto [ptr, ec] = std::from_chars(hex.data() + i, hex.data() + i + 2, byte, 16);

			if (ec == std::errc()) bytes.push_back(byte);

		}

		return bytes;
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

	std::expected<void, Error> RabbitManager::Conductor( std::string_view inputPath, std::string_view outputPath, std::string_view keyPath, const Options& opt ) {

		std::vector< uint8_t > KeyData;

		if ( opt.generate_key ) {

			auto genKey = KeyManager::GenerateKey();
			KeyData = std::vector<uint8_t>( genKey.begin(), genKey.end() );
			auto writeKeyData = KeyManager::WriteKey( keyPath, KeyData );
			if ( !writeKeyData ) return std::unexpected( writeKeyData.error() );

		}
		else {
			if ( !fs::exists( keyPath ) ) {

				if ( ask_yes_no( "Файл ключа не знайдено. Згенерувати новий?", opt.yes_to_all ) ) {

					auto genKey = KeyManager::GenerateKey();
					KeyData = std::vector<uint8_t>( genKey.begin(), genKey.end() );
					auto writeKeyData = KeyManager::WriteKey( keyPath, KeyData );
					if ( !writeKeyData ) return std::unexpected( writeKeyData.error() );

				}
				else {

					return std::unexpected( Error::ActionAborted );
				}
			}
			else {

				auto readKey = KeyManager::ReadKey( keyPath );
				if ( !readKey ) return std::unexpected( readKey.error() );	

				KeyData = readKey.value();

			}
	
		}

		std::span<const uint8_t, Rabbit::KEY_SIZE> K( KeyData.data(), Rabbit::KEY_SIZE );

		if ( opt.encrypt_mode ) {

			std::vector<uint8_t> iv_to_use;

			if ( !opt.no_iv ) {

				bool generate_new_iv = true;

				if ( KeyData.size() >= KeyManager::FULL_KEY_SIZE ) {

					if ( opt.yes_to_all ) {

						iv_to_use = std::vector<uint8_t>( KeyData.begin() + Rabbit::KEY_SIZE, KeyData.begin() + KeyManager::FULL_KEY_SIZE );
						generate_new_iv = false;

					}

					else {

						if ( ask_yes_no( "У файлі ключа вже є IV. Використовувати його?", opt.yes_to_all ) ) {

							iv_to_use = std::vector<uint8_t>( KeyData.begin() + Rabbit::KEY_SIZE, KeyData.begin() + KeyManager::FULL_KEY_SIZE );
							generate_new_iv = false;
						}
						else {
							if ( !ask_yes_no( "Згенерувати новий IV?", false ) ) return std::unexpected( Error::ActionAborted );
						}

					}

				}

				if (generate_new_iv) {

					auto genIV = KeyManager::GenerateIV();
					iv_to_use = std::vector<uint8_t>( genIV.begin(), genIV.end() );

					bool save = opt.save_iv_to_key || ( !opt.yes_to_all && ask_yes_no( "Зберегти цей новий IV у файл ключа (для сумісності зі старими версіями)?", false ) );
					if (save) KeyManager::AppendIVToKey( std::string( keyPath ), iv_to_use );

				}
			}

			std::vector<uint8_t> zero_iv( Rabbit::IV_SIZE, 0 );

			if ( !opt.no_iv ) rabbit.Init( static_cast<std::span<const uint8_t, Rabbit::KEY_SIZE>>(K), std::span<const uint8_t, Rabbit::IV_SIZE>(iv_to_use.data(), Rabbit::IV_SIZE) );
			else rabbit.Init( static_cast<std::span<const uint8_t, Rabbit::KEY_SIZE>>( K ), std::span<const uint8_t, Rabbit::IV_SIZE>( zero_iv.data(), Rabbit::IV_SIZE ) );


			std::ifstream inputFile( inputPath.data(), std::ios::binary );

			if ( !inputFile ) return std::unexpected( Error::NullInput );

			std::ofstream outputFile( outputPath.data(), std::ios::binary );

			if ( !outputFile ) return std::unexpected( Error::FileWriteError );

			if ( !opt.no_iv ) {

				std::string header = std::format( "IV: {}\n", bytes_to_hex( iv_to_use ) );
				outputFile.write( header.c_str(), header.size() );
			}

			ProcessStream( rabbit, inputFile, outputFile );
		}

		else {
		
			std::ifstream inputFile( std::string( inputPath ), std::ios::binary );
			if ( !inputFile ) return std::unexpected( Error::NullInput );

			std::vector<uint8_t> file_iv;
			std::vector<uint8_t> key_iv;

			char header_buf[21];
			inputFile.read( header_buf, 21 );

			if (inputFile.gcount() == 21 && std::string_view( header_buf, 4 ) == "IV: " && header_buf[20] == '\n') {

				file_iv = hex_to_bytes( std::string( header_buf + 4, 16 ) );
				if ( file_iv.size() != Rabbit::IV_SIZE ) {

					file_iv.clear();
					inputFile.clear(); 
					inputFile.seekg(0);

				}
			}

			else {

				inputFile.clear(); 
				inputFile.seekg(0);

			}

			if ( KeyData.size() >= KeyManager::FULL_KEY_SIZE ) {

				key_iv = std::vector<uint8_t>( KeyData.begin() + Rabbit::KEY_SIZE, KeyData.begin() + KeyManager::FULL_KEY_SIZE );

			}

			bool use_iv = false;
			std::vector<uint8_t> iv_to_use;

			if ( opt.no_iv ) {

				use_iv = false;
				inputFile.clear();
				inputFile.seekg(0);

			}
			else if ( !file_iv.empty() && key_iv.empty() ) {

				iv_to_use = file_iv; 
				use_iv = true;
			}
			else if ( file_iv.empty() && !key_iv.empty() ) {

				iv_to_use = key_iv; 
				use_iv = true;
			}

			else if ( !file_iv.empty() && !key_iv.empty() ) {

				if ( file_iv == key_iv ) {

					iv_to_use = file_iv; 
					use_iv = true;

				}
				else {
					if ( opt.force_iv_from == "file" ) {

						iv_to_use = file_iv; 
						use_iv = true;
					}
					else if ( opt.force_iv_from == "key" ) {

						iv_to_use = key_iv; 
						use_iv = true;
					}
					else {

						std::println( "\nЗнайдено різні вектори ініціалізації." );
						int choice = ask_choice( "1 - З метаданих файлу;\n2 - З файлу ключа\n ПИК: ", 1, opt.yes_to_all );
						iv_to_use = ( choice == 2 ) ? key_iv : file_iv;
						use_iv = true;

					}
				}
			}
			else {
				if ( !opt.yes_to_all && !ask_yes_no( "\nВектор ініціалізації не знайдено. Спробувати розшифрувати тільки за ключем?", false ) ) {
					return std::unexpected( Error::ActionAborted );
				}

				use_iv = false;

			}

			std::vector<uint8_t> IV_(Rabbit::IV_SIZE, 0);
			if ( use_iv ) rabbit.Init( K, std::span<const uint8_t, Rabbit::IV_SIZE>(iv_to_use.data(), Rabbit::IV_SIZE) );
			else rabbit.Init( K, std::span<const uint8_t, Rabbit::IV_SIZE>( IV_.data(), Rabbit::IV_SIZE ) );
			

			std::ofstream outputFile( std::string( outputPath ), std::ios::binary );
			if ( !outputFile ) return std::unexpected( Error::FileWriteError );

			ProcessStream( rabbit, inputFile, outputFile );

		}

		return {};
	}

	std::expected<void, Error> RabbitManager::ProcessStream( Rabbit& cipher, std::ifstream& in, std::ofstream& out ) {

		std::vector<uint8_t> buffer( BUFFER_SIZE );

		while ( in.read(reinterpret_cast<char*>( buffer.data() ), BUFFER_SIZE) || in.gcount() > 0 ) {

			std::streamsize bytesRead = in.gcount();

			if ( bytesRead > 0 ) {

				std::span<uint8_t> dataSpan( buffer.data(), static_cast<size_t>( bytesRead ) );
				cipher.XorData(dataSpan, dataSpan);
				out.write( reinterpret_cast<const char*>( dataSpan.data() ), dataSpan.size() );

				if ( !out ) return std::unexpected( Error::FileWriteError );
			}
		}

		in.close();
		out.close();

		return {};
	}