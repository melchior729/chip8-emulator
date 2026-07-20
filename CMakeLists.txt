cmake_minimum_required(VERSION 3.28...4.2)
project(chip8emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

file(GLOB SRCS "src/*")

add_executable(chip8 ${SRCS})
target_sources(chip8 PRIVATE ${SRCS})

find_package(SDL3 REQUIRED)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)
target_compile_definitions(chip8 PRIVATE SDL_MAIN_USE_CALLBACKS)

enable_testing()
add_subdirectory(tests)
