#include "../include/Errors.hpp"

std::string_view error_to_string(Error err) {
    switch (err) {
    case Error::ParseError:     return "Помилка розбору.";
    default: 				    return "Невідома помилка.";
    }
}