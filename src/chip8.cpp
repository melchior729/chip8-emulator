/// @file chip8.hpp
/// @brief implementation of the Chip8 class
/// @author Abhay Manoj
/// @date Feb 21 2026
#include "chip8.hpp"
#include <cstdint>
#include <random>

static uint8_t generate_random_uint8() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 0xF);
  return dist(gen);
}

void Chip8::sys(const uint16_t address) { PC = address & 0x0FFF; }

void Chip8::cls() { display_buffer.fill(0); }

void Chip8::ret() {
  SP--;
  PC = stack[SP];
}

void Chip8::jump(const uint16_t address) { PC = address & 0x0FFF; }

void Chip8::call(const uint16_t address) {
  stack[SP] = PC + 2;
  PC = address & 0x0FFF;
  SP++;
}

void Chip8::skip_next_if_equal_byte(const uint8_t register_num,
                                    const uint8_t byte) {
  if (register_num == byte) {
    PC += 2;
  }
}

void Chip8::skip_next_if_not_equal_byte(uint8_t register_num, uint8_t byte) {
  if (register_num != byte) {
    PC += 2;
  }
}

void Chip8::skip_next_if_equal_registers(uint8_t register_x,
                                         uint8_t register_y) {
  if (register_x == register_y) {
    PC += 2;
  }
}

void Chip8::load_from_byte(uint8_t register_num, uint8_t byte) {
  V[register_num] = byte;
}

void Chip8::add(uint8_t register_num, uint8_t byte) { V[register_num] += byte; }

void Chip8::load_from_register_to_register(uint8_t register_x,
                                           uint8_t register_y) {
  V[register_x] = V[register_y];
}

void Chip8::bitwise_or(uint8_t register_x, uint8_t register_y) {
  V[register_x] |= V[register_y];
}

void Chip8::bitwise_and(uint8_t register_x, uint8_t register_y) {
  V[register_x] &= V[register_y];
}

void Chip8::bitwise_xor(uint8_t register_x, uint8_t register_y) {
  V[register_x] ^= V[register_y];
}

void Chip8::add_and_store_carry(uint8_t register_x, uint8_t register_y) {
  uint16_t sum = register_x + register_y;
  V[0xF] = (sum > 0xFF) ? 1 : 0;
  V[register_x] = sum & 0xFF;
}

void Chip8::subtract(uint8_t register_x, uint8_t register_y) {
  V[register_x] = register_x - register_y;
  V[0xF] = register_x >= register_y;
}

void Chip8::shift_right(uint8_t register_num) { V[register_num] >>= 1; }

void Chip8::reverse_subtract(uint8_t register_x, uint8_t register_y) {
  V[register_x] = V[register_y] - V[register_x];
  V[0xF] = register_y >= register_x;
}

void Chip8::shift_left(uint8_t register_num) { V[register_num] <<= 1; }

void Chip8::skip_next_if_not_equal_registers(uint8_t register_x,
                                             uint8_t register_y) {
  if (V[register_x] != V[register_y]) {
    PC += 2;
  }
}

void Chip8::load_I(uint16_t address) { I = address & 0xFFF; }

void Chip8::jump_off_register(uint16_t address) { PC = V[0] + address; }

void Chip8::rand(uint8_t register_num, uint8_t byte) {
  V[register_num] = generate_random_uint8() & byte;
}

void Chip8::draw(uint8_t register_x, uint8_t register_y, uint8_t nibble) {}

void Chip8::skip_if_pressed(uint8_t register_num) {
  if (keypad[V[register_num]]) {
    PC += 2;
  }
}

void Chip8::skip_if_not_pressed(uint8_t register_num) {
  if (!keypad[V[register_num]]) {
    PC += 2;
  }
}

void Chip8::load_from_delay_timer(uint8_t register_x) {}

void Chip8::store_key_press(uint8_t register_num) {}

void Chip8::set_delay_timer(uint8_t register_num) { DT = V[register_num]; }

void Chip8::set_sound_timer(uint8_t register_num) { ST = V[register_num]; }

void Chip8::add_I(uint8_t register_num) { I += V[register_num]; }

void Chip8::load_sprite(uint8_t register_num) {}

void Chip8::write_binary_coded_decimal(uint8_t register_num) {}

void Chip8::store_memory_from_registers(uint8_t register_num) {}

void Chip8::store_registers_from_memory(uint8_t register_num) {}
