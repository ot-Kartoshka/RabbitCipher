#pragma once

#include <string_view>


enum class Error {
    ParseError,
    InvalidKeySize,
    InvalidIVSize,
    NullData,
    NullInput,
    FileWriteError,
    NullKeyInput,
    OverwriteInputError,
    ActionAborted,
    LogicError
};

std::string_view error_to_string(Error err);