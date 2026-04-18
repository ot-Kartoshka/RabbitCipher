#include <fstream>

std::expected<void, Error> ReadFile(std::string_view path);

std::expected<void, Error> WriteFile(std::string_view path);