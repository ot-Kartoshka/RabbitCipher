#include <string>
#include <filesystem>

#include "include/RabbitManager.hpp"
#include "include/KeyManager.hpp"
#include "include/Errors.hpp"

namespace fs = std::filesystem;

struct Args {
    std::string input_path;
    std::string output_path;
    std::string key_path;
    bool generate_key = false;
    bool encrypt_mode = true;
    bool help = false;
    bool no_iv = false;
    bool save_iv_to_key = false;
    std::string force_iv_from = "";
    bool yes_to_all = false;
};


std::string ask_for_path( std::string_view message, std::string_view default_val ) {
    std::print( "{} [{}]: ", message, default_val );
    std::string input;
    std::getline( std::cin, input );
    return input.empty() ? std::string( default_val ) : input;
}


bool ask_yes_no( std::string_view message ) {
    std::print( "{} (y/N): ", message );
    std::string input;
    std::getline( std::cin, input );
    return !input.empty() && ( input[0] == 'y' || input[0] == 'Y' || input[0] == 'т' || input[0] == 'Т' );
}

std::expected<void, Error> resolve_file_conflict( std::string& target_path, const std::string& input_path ) {
    if ( fs::exists( target_path ) && fs::equivalent( target_path, input_path ) ) {
        return std::unexpected( Error::OverwriteInputError );
    }

    if ( fs::exists(target_path) ) {
        std::println( "Попередження: Файл '{}' вже існує.", target_path );

        if ( ask_yes_no( "Бажаєте перезаписати його?" ) ) {
            return {};
        }


        std::print( "Введіть нову назву (або Enter для скасування): " );
        std::string new_path;
        std::getline( std::cin, new_path );

        if ( new_path.empty() ) return std::unexpected( Error::ActionAborted );
        target_path = new_path;
        return resolve_file_conflict( target_path, input_path );
    }

    return {};
}

std::expected<Args, Error> parse_args( int argc, char** argv ) {
    Args args;
    std::vector<std::string_view> v_args( argv + 1, argv + argc );
    bool key_flag = false;

    for ( size_t i = 0; i < v_args.size(); ++i ) {
        std::string_view curr = v_args[i];

        if (curr == "-h" || curr == "--help") args.help = true;
        else if (curr == "-e" || curr == "--encrypt") args.encrypt_mode = true;
        else if (curr == "-d" || curr == "--decrypt") args.encrypt_mode = false;
        else if (curr == "-y" || curr == "--yes") args.yes_to_all = true;
        else if (curr == "--no-iv") args.no_iv = true;
        else if (curr == "--save-iv-to-key") args.save_iv_to_key = true;  
        else if (curr == "--force-iv-from" && i + 1 < v_args.size()) args.force_iv_from = v_args[++i]; 
        else if ( curr == "-g" || curr == "--generate") {
            if ( key_flag ) return std::unexpected( Error::ParseError );
            args.generate_key = true;
            if ( i + 1 < v_args.size() && v_args[i + 1][0] != '-' ) args.key_path = v_args[++i];
        }
        else if ( curr == "-k" || curr == "--key") {
            if ( args.generate_key ) return std::unexpected( Error::ParseError );
            if ( i + 1 < v_args.size() ) args.key_path = v_args[++i];
            key_flag = true;
        }
        else if ( ( curr == "-i" || curr == "--input" ) && i + 1 < v_args.size() ) args.input_path = v_args[++i];
        else if ( ( curr == "-o" || curr == "--output" ) && i + 1 < v_args.size() ) args.output_path = v_args[++i];
        else return std::unexpected( Error::ParseError );
    }

    if ( !args.encrypt_mode && args.generate_key ) {
        return std::unexpected( Error::LogicError );
    }

    if ( !args.help && args.input_path.empty() ) return std::unexpected( Error::NullInput );
    return args;
}

void PrintHelp() {
    std::println("                         Rabbit Cipher CLI");
    std::println("Використання:");
    std::println("  RabbitCipher -i <input> [ОПЦІЇ]");
    std::println("");
    std::println("Обов'язкові параметри:");
    std::println("  -i, --input <path>     Шлях до вхідного файлу для обробки.");
    std::println("");
    std::println("Режими роботи (за замовчуванням -e):");
    std::println("  -e, --encrypt          Зашифрувати вхідний файл.");
    std::println("  -d, --decrypt          Розшифрувати вхідний файл.");
    std::println("");
    std::println("Робота з ключем та IV:");
    std::println("  -k, --key <path>       Використати існуючий файл ключа.");
    std::println("  -g, --generate [path]  Згенерувати новий ключ.");
    std::println("  --no-iv                Шифрувати/Дешифрувати без вектора ініціалізації (тільки ключ).");
    std::println("  --save-iv-to-key       Зберегти згенерований IV також у файл ключа (тільки шифрування).");
    std::println("  --force-iv-from <src>  Джерело IV при конфлікті: 'file' або 'key' (тільки дешифрування).");
    std::println("  -y, --yes              Тихий режим (автоматично погоджуватись на всі запитання).");
    std::println("");
    std::println("Додаткові опції:");
    std::println("  -o, --output <path>    Шлях для збереження результату.");
    std::println("  -h, --help             Показати цю довідку.");
}

int main( int argc, char** argv ) {
    auto parsedArgs = parse_args( argc, argv );
    if ( !parsedArgs ) {
        PrintHelp();
        std::println( stderr, "Error: {}", error_to_string( parsedArgs.error() ) );
        return 1;
    }

    Args args = parsedArgs.value();
    if ( args.help ) {
        PrintHelp();
        return 0;
    }

    if ( args.generate_key && args.key_path.empty() ) {
        std::string def_key = fs::path( args.input_path ).stem().string() + ".key";
        args.key_path = ask_for_path( "Вкажіть назву для файлу ключа", def_key );
    }

    if (auto status = resolve_file_conflict( args.key_path, args.input_path ); !status) {
        std::println( stderr, "Error: {}", error_to_string( status.error() ) );
        return 1;
    }


    if ( args.output_path.empty() ) {
        std::string suffix = args.encrypt_mode ? "_encrypted" : "_decrypted";
        std::string def_out = fs::path( args.input_path ).stem().string() + suffix + ".bin";
        args.output_path = ask_for_path( "Назва вихідного файлу", def_out );
    }

    if (auto status = resolve_file_conflict( args.output_path, args.input_path ); !status) {
        std::println( stderr, "Error: {}", error_to_string( status.error() ) );
        return 1;
    }


    std::error_code ec;
    auto file_size = fs::file_size( args.input_path, ec );
    if ( ec ) {
        std::println( stderr, "Не вдалося отримати доступ до вхідного файлу: {}", ec.message() );
        return 1;
    }

    RabbitManager::Options opt;
    opt.encrypt_mode = args.encrypt_mode;
    opt.generate_key = args.generate_key;
    opt.no_iv = args.no_iv;
    opt.save_iv_to_key = args.save_iv_to_key;
    opt.force_iv_from = args.force_iv_from;
    opt.yes_to_all = args.yes_to_all;

    RabbitManager manager;
    auto processResult = manager.Conductor( args.input_path, args.output_path, args.key_path, opt );

    if ( !processResult ) {
        std::println( stderr, "Error during processing: {}", error_to_string( processResult.error() ) );

        if ( fs::exists( args.output_path ) && ask_yes_no( "Видалити пошкоджений/неповний вихідний файл?" ) ) {
            fs::remove( args.output_path );
            std::println( "Файл видалено." );
        }
        return 1;
    }

    std::println( "\nФайл '{}' ({} байт) успішно {} у '{}'.", args.input_path, file_size, ( args.encrypt_mode ? "зашифровано" : "дешифровано" ), args.output_path );

    if ( ask_yes_no( "Видалити оригінальний вхідний файл?" ) ) {
        fs::remove( args.input_path );
        std::println( "Оригінал видалено." );
    }

    return 0;
}