# CMakeList.txt: проект CMake для Rabbit; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Rabbit")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Rabbit "main.cpp" "src/Errors.cpp" "src/FileHandler.cpp" "src/Rabbit.cpp" "include/Errors.hpp" "include/FileHandler.hpp" "include/Rabbit.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Rabbit PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
