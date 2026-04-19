#include "../include/Errors.hpp"

std::string_view error_to_string(Error err) {
    switch (err) {
    case Error::ParseError:			 return "Помилка розбору.";
	case Error::InvalidKeySize:		 return "Невірний розмір ключа.";
	case Error::InvalidIVSize: 		 return "Невірний розмір IV.";
	case Error::NullData:			 return "Невірні дані.";
	case Error::NullInput:			 return "Невірний вхід.";
	case Error::FileWriteError:		 return "Помилка запису в файл.";
	case Error::NullKeyInput:		 return "Невірний ключ.";   
	case Error::OverwriteInputError: return "Неможливо перезаписати вхідний файл.";
	case Error::ActionAborted:		 return "Дія скасована користувачем.";
	case Error::LogicError:			 return "Логічна помилка в аргументах (генерація ключа недопустима в режимі розшифрування).";
    default: 						 return "Невідома помилка.";
    }
}