/// @file chip8.hpp
/// @brief implementation of the Chip8 class
/// @author Abhay Manoj
/// @date Feb 21 2026
#include "chip8.hpp"
#include <cstdint>
#include <random>

/// @brief generates a random uint8_t, 0 - 255.
/// @return the randomly generated uint8
static uint8_t generate_random_uint8() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 0xFF);
  return dist(gen);
}

Chip8::Chip8() {
  memory.fill(0);
  load_font_data();
}

Chip8::Chip8(const std::array<uint8_t, MEMORY_SIZE> &memory) {
  load_into_memory(memory);
  load_font_data();
}

void Chip8::load_into_memory(const std::array<uint8_t, MEMORY_SIZE> &memory) {
  this->memory = memory;
}

void Chip8::cycle() {}

void Chip8::load_font_data() {
  static const std::array<uint8_t, 80> fontset = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  std::copy(fontset.begin(), fontset.end(), memory.begin());
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
  if (V[register_num] == byte) {
    PC += 2;
  }
}

void Chip8::skip_next_if_not_equal_byte(uint8_t register_num, uint8_t byte) {
  if (V[register_num] != byte) {
    PC += 2;
  }
}

void Chip8::skip_next_if_equal_registers(uint8_t register_x,
                                         uint8_t register_y) {
  if (V[register_x] == V[register_y]) {
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
  uint16_t sum = V[register_x] + V[register_y];
  V[0xF] = (sum > 0xFF) ? 1 : 0;
  V[register_x] = sum & 0xFF;
}

void Chip8::subtract(uint8_t register_x, uint8_t register_y) {
  V[0xF] = V[register_x] >= V[register_y];
  V[register_x] = V[register_x] - V[register_y];
}

void Chip8::shift_right(uint8_t register_num) {
  V[0xF] = V[register_num] & 1;
  V[register_num] >>= 1;
}

void Chip8::reverse_subtract(uint8_t register_x, uint8_t register_y) {
  V[0xF] = V[register_y] >= V[register_x];
  V[register_x] = V[register_y] - V[register_x];
}

void Chip8::shift_left(uint8_t register_num) {
  V[0xF] = (V[register_num] >> 7) & 1;
  V[register_num] <<= 1;
}

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

void Chip8::draw(uint8_t register_x, uint8_t register_y, uint8_t height) {
  V[0xF] = 0;

  for (uint8_t row = 0; row < height; row++) {
    uint8_t byte = memory[I + row];
    uint16_t y = (V[register_y] + row) % HEIGHT;
    uint16_t offset = y * WIDTH;

    for (uint8_t col = 0; col < SPRITE_WIDTH; col++) {
      uint8_t bit = byte >> (SPRITE_WIDTH - col - 1) & 1;
      if (!bit) {
        continue;
      }

      uint16_t x = (V[register_x] + col) % WIDTH;
      uint8_t *pixel = &display_buffer[x + offset];

      if (*pixel) {
        V[0xF] = 1;
      }

      *pixel ^= 1;
    }
  }
}

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

void Chip8::load_from_delay_timer(uint8_t register_x) { V[register_x] = DT; }

void Chip8::store_key_press(uint8_t register_num) {
  waiting_for_input = true;
  target_register = register_num;
}

void Chip8::set_delay_timer(uint8_t register_num) { DT = V[register_num]; }

void Chip8::set_sound_timer(uint8_t register_num) { ST = V[register_num]; }

void Chip8::add_I(uint8_t register_num) { I += V[register_num]; }

void Chip8::load_sprite(uint8_t register_num) {
  constexpr uint8_t SPRITE_HEIGHT = 5;
  I = V[register_num] * SPRITE_HEIGHT;
}

void Chip8::write_binary_coded_decimal(uint8_t register_num) {
  memory[I] = V[register_num] / 100;
  memory[I + 1] = (V[register_num] / 10) % 10;
  memory[I + 2] = V[register_num] % 10;
}

void Chip8::store_memory_from_registers(uint8_t register_num) {
  uint16_t ptr = I;
  for (uint8_t i = 0; i <= register_num; i++) {
    memory[ptr++] = V[i];
  }
}

void Chip8::store_registers_from_memory(uint8_t register_num) {
  uint16_t ptr = I;
  for (uint8_t i = 0; i <= register_num; i++) {
    V[i] = memory[ptr++];
  }
}

const std::array<uint8_t, Chip8::WIDTH * Chip8::HEIGHT> &
Chip8::get_display_buffer() const {
  return display_buffer;
}

const std::array<uint16_t, Chip8::STACK_SIZE> &Chip8::get_stack() const {
  return stack;
}

const std::array<uint8_t, Chip8::REGISTER_COUNT> &Chip8::get_registers() const {
  return V;
}

uint8_t Chip8::get_register(uint8_t register_num) const {
  return V[register_num];
}

uint16_t Chip8::get_I() const { return I; }

uint16_t Chip8::get_PC() const { return PC; }

uint8_t Chip8::get_SP() const { return SP; }

uint8_t Chip8::get_DT() const { return DT; }

void Chip8::set_DT(uint8_t value) { DT = value; }

uint8_t Chip8::get_ST() const { return ST; }

void Chip8::set_ST(uint8_t value) { ST = value; }

void Chip8::set_keypad(uint8_t keypad_num, uint8_t status) {
  keypad[keypad_num] = status;
}

void Chip8::reset() {
  stack.fill(0);
  V.fill(0);
  keypad.fill(0);
  display_buffer.fill(0);
  memory.fill(0);
  I = 0;
  PC = START;
  SP = 0;
  DT = 0;
  ST = 0;
  target_register = 0;
  waiting_for_input = 0;
  load_font_data();
}
