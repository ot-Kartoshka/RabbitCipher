cmake_minimum_required (VERSION 3.28)
project ("RabbitCipherProject" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


add_library(RabbitCore STATIC
    "src/Errors.cpp"
    "src/KeyManager.cpp"
    "src/Rabbit.cpp"
    "src/RabbitManager.cpp"
)
target_include_directories(RabbitCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_executable(Rabbit "main.cpp")
target_link_libraries(Rabbit PRIVATE RabbitCore)


include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(RabbitTests "tests/test_rabbit.cpp")
target_link_libraries(RabbitTests PRIVATE RabbitCore gtest_main)

include(GoogleTest)
gtest_discover_tests(RabbitTests)