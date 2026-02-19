/// @file chip8.hpp
/// @brief declaration of the chip8 class
/// @author Abhay Manoj
/// @date Feb 19 2026
#pragma once

#include <array>
#include <cstdint>

class Chip8 {
private:
  static constexpr int STACK_SIZE = 16;
  static constexpr int REGISTER_COUNT = 16;
  static constexpr int KEYPAD_OPTIONS = 16;
  static constexpr int MEMORY_SIZE = 4096;
  static constexpr int STARTING_ADDRESS = 0x200;
  static constexpr int WIDTH = 64;
  static constexpr int HEIGHT = 32;
  static constexpr int FREQUENCY = 432; // audio frequency

  std::array<uint16_t, STACK_SIZE> stack{};     // stores return addresses
  std::array<uint8_t, REGISTER_COUNT> V{};      // registers 0 - F
  std::array<uint8_t, KEYPAD_OPTIONS> keypad{}; // status of keypad buttons
  std::array<uint8_t, WIDTH * HEIGHT>
      display_buffer{}; // pixel values, on or off
  std::array<uint8_t, MEMORY_SIZE> memory{};

  uint16_t I = 0;                 // stores memory addresses, use 12 lowest bits
  uint16_t PC = STARTING_ADDRESS; // currently executing address
  uint8_t SP = 0;                 // topmost level of the stack
  uint8_t DT = 0;                 // delay timer register
  uint8_t ST = 0;                 // sound timer register, play if > 0

  /// @brief jumps to a routine at nnn
  /// 0NNN
  /// @param address 0nnn
  void sys(uint16_t address);

  /// @brief clears the screen
  /// 00E0
  void cls();

  /// @brief returns from a subroutine
  /// 00EE
  void ret();

  /// @brief jumps to address at nnn
  /// 1NNN
  /// @param address 0nnn
  void jump(uint16_t address);

  /// @brief calls the routine at nnn
  /// 2NNN
  /// @param address 0nnn
  void call(uint16_t address);

  /// @brief skips next instruction if V_num == byte.
  /// 3XNN
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void skip_next_if_equal_byte(uint8_t register_num, uint8_t byte);

  /// @brief skips next instruction if V_x != byte.
  /// 4XNN
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void skip_next_if_not_equal_byte(uint8_t register_num, uint8_t byte);

  /// @brief skips next instruction if V_x == V_y.
  /// 5XY0
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void skip_next_if_equal_register(uint8_t register_x, uint8_t register_y);

  /// @brief stores value of byte in V_x.
  /// 6XNN
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void load_from_byte(uint8_t register_num, uint8_t byte);

  /// @brief adds V_x + byte, and stores in V_x
  /// 7XNN
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void add(uint8_t register_num, uint8_t byte);

  /// @brief stores value of V_y in V_x.
  /// 8XY0
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void load_from_register(uint8_t register_x, uint8_t register_y);

  /// @brief performs bitwise OR on V_x and V_y, stores in V_x
  /// 8XY1
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_or(uint8_t register_x, uint8_t register_y);

  /// @brief performs bitwise AND on V_x and V_y, stores in V_x
  /// 8XY2
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_and(uint8_t register_x, uint8_t register_y);

  /// @brief performs bitwise XOR on V_x and V_y, stores in V_x
  /// 8XY3
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_xor(uint8_t register_x, uint8_t register_y);

  /// @brief stores V_x + V_y in V_x, sets V_F to the carry
  /// 8XY4
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void add_and_store_carry(uint8_t register_x, uint8_t register_y);

  /// @brief stores V_x - V_y in V_x, sets V_F to not borrow
  /// 8XY5
  /// V_F being 1 implies that the difference is non-negative
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void subtract(uint8_t register_x, uint8_t register_y);

  /// @brief stores V_x >> 1 in V_x
  /// 8XY6
  /// @param register_num the register number, x in V_x
  void shift_right(uint8_t register_num);

  /// @brief stores V_y - V_x in V_x, sets V_F to not borrow
  /// 8XY7
  /// V_F being 1 implies that the difference is non-negative
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void reverse_subtract(uint8_t register_x, uint8_t register_y);

  /// @brief stores V_x << 1 in V_x
  /// 8XYE
  /// @param register_num the register number, x in V_x
  void shift_left(uint8_t register_num);

  /// @brief skips next instruction if V_x != V_y.
  /// 9XY0
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void skip_next_if_not_equal_register(uint8_t register_x, uint8_t register_y);

  /// @brief loads nnn into I
  /// ANNN
  /// @param address 0nnn
  void load_I(uint16_t address);

  /// @brief jumps to location nnn + V_x
  /// BNNN
  /// @param register_num the register number, x in V_x
  /// @param address 0nnn
  void jump_off_register(uint8_t register_num, uint16_t address);

  /// @brief generates random byte and ANDed with provided byte, stored in V_x
  /// CXNN
  /// @param register_num the register number, x in V_x
  /// @param byte the value to and with
  void rand(uint8_t register_num, uint8_t byte);

  /// @brief displays n byte sprite starting at I at (V_x, V_y), V_F =
  /// collision.
  /// DXYN
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  /// @param nibble the height of the sprite to draw
  void draw(uint8_t register_x, uint8_t register_y, uint8_t nibble);

  /// @brief skips the next instruction if key of V_x is pressed
  /// EX9E
  /// @param register_num the register number, x in V_x
  void skip_if_pressed(uint8_t register_num);

  /// @brief skips the next instruction if key of V_x is not pressed
  /// EXA1
  /// @param register_num the register number, x in V_x
  void skip_if_not_pressed(uint8_t register_num);

  /// @brief loads the value from the delay timer to V_x
  /// FX07
  /// @param register_num the register number, x in V_x
  void load_from_delay_timer(uint8_t register_num);

  /// @brief execution stops until a key is pressed; stored in V_x
  /// FX0A
  /// @param register_num the register number, x in V_x
  void store_key_press(uint8_t register_num);

  /// @brief sets the value of the delay timer to V_x
  /// FX15
  /// @param register_num the register number, x in V_x
  void set_delay_timer(uint8_t register_num);

  /// @brief sets the value of the sound timer to V_x
  /// FX18
  /// @param register_num the register number, x in V_x
  void set_sound_timer(uint8_t register_num);

  /// @brief adds I + V_x and stores in the I register
  /// FX1E
  /// @param register_num the register number, x in V_x
  void add_I(uint8_t register_num);

  /// @brief sets I to the location of the sprite in V_x
  /// FX29
  /// @param register_num the register number, x in V_x
  void load_sprite(uint8_t register_num);

  /// @brief writes the BCD form of V_x in locations I, I+1, I+2
  /// FX33
  /// @param register_num the register number, x in V_x
  void write_binary_coded_decimal(uint8_t register_num);

  /// @brief stores registers V_0 to V_x into memory, starting at I
  /// FX55
  /// @param register_num the register number to stop at, x in V_x
  void store_memory_from_registers(uint8_t register_num);

  /// @brief stores memory starting at I into V_0 to V_x
  /// FX65
  /// @param register_num the register number
  void store_registers_from_memory(uint8_t register_num);

public:
  /// @brief default constructor, does not have defined memory
  Chip8();

  /// @brief constructs a Chip8 with preloaded memory
  /// @param memory the memory to initialize with
  explicit Chip8(std::array<uint8_t, MEMORY_SIZE> memory);

  /// @brief replaces the Chip8's memory with the provided one
  /// @param memory the new memory to use
  void load_into_memory(std::array<uint8_t, MEMORY_SIZE> memory);

  /// @brief performs one cpu tick
  void cycle();

  /// @brief returns the display buffer, 64 x 32
  /// @return the display buffer
  std::array<uint8_t, WIDTH * HEIGHT> get_display_buffer();

  /// @brief returns the stack
  /// @return the stack
  std::array<uint16_t, STACK_SIZE> get_stack();

  /// @brief returns the list of registers
  /// @return the list of registers
  std::array<uint8_t, REGISTER_COUNT> get_registers();

  /// @brief returns a specific register
  /// @param register_num the register number
  /// @return the content of the register
  uint8_t get_register(uint8_t register_num);

  /// @brief returns the content of I register
  /// @return the I register
  uint16_t get_I();

  /// @brief returns the content of the program counter register
  /// @return the content of the program counter reigster
  uint16_t get_PC();

  /// @brief returns the content of the stack pointer register
  /// @return the content of the stack pointer register
  uint8_t get_SP();

  /// @brief returns the content of the delay time register
  /// @return the content of the delay time register
  uint8_t get_DT();

  /// @brief returns the content of the sound timer register
  /// @return the content of the sound timer register
  uint8_t get_ST();

  /// @brief sets the keypad value to the status value
  void set_keypad(uint8_t register_num, uint8_t status);
};
