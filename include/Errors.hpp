#pragma once
#include <string_view>


enum class Error {
    ParseError,
};

std::string_view error_to_string(Error err);