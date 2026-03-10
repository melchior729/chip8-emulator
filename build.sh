#!/bin/bash

mkdir -p web

# CHANGE: We are now outputting to index.js, not index.html
emcc src/main.cpp src/core/chip8.cpp \
  -Isrc/core \
  -o web/index.js \
  -sUSE_SDL=3 \
  --embed-file roms/ \
  -sALLOW_MEMORY_GROWTH=1 \
  -std=c++20 \
  -O0 \
  -DSDL_MAIN_USE_CALLBACKS=1
