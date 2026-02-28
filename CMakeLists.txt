cmake_minimum_required(VERSION 3.28...4.2)
project(chip8emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chip8lib src/chip8.cpp)
target_include_directories(chip8lib PUBLIC src)

add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 PRIVATE chip8lib)

find_package(SDL3 REQUIRED)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)
target_compile_definitions(chip8 PRIVATE SDL_MAIN_USE_CALLBACKS)

enable_testing()
add_subdirectory(tests)
