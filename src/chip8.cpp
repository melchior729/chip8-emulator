#include "chip8.hpp"

Chip8::Chip8() {}
Chip8::Chip8(std::array<uint8_t, Chip8::MEMORY_SIZE> memory) {
  load_into_memory(memory);
}

void Chip8::load_into_memory(std::array<uint8_t, Chip8::MEMORY_SIZE> memory) {
  this->memory = memory;
}

void Chip8::sys(uint16_t address) { this->PC = address & 0xFFF; }
