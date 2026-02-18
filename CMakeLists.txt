cmake_minimum_required(VERSION 3.28...4.2)
project(chip8emulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chip8lib src/Chip8.cpp)
target_include_directories(chip8lib PUBLIC src)

add_executable(chip8 src/main.cpp)
target_include_directories(chip8 PRIVATE chip8lib)

enable_testing()
add_subdirectory(tests)
