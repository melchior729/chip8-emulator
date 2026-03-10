#!/bin/bash

mkdir -p web

emcc src/main.cpp src/core/chip8.cpp \
  -Isrc/core \
  -o web/index.js \
  -sUSE_SDL=3 \
  --embed-file roms/ \
  -sALLOW_MEMORY_GROWTH=1 \
  -std=c++20 \
  -O0 \
  -DSDL_MAIN_USE_CALLBACKS=1
