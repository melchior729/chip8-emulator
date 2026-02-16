cmake_minimum_required(VERSION 3.28...4.2)
project(chip8emulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chip8 src/main.cpp)
