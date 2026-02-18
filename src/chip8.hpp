#pragma once

#include <array>
#include <cstdint>

class Chip8 {
private:
  static constexpr int STACK_SIZE = 16;
  static constexpr int REGISTER_COUNT = 16;
  static constexpr int MEMORY_SIZE = 4096;
  static constexpr int STARTING_ADDRESS = 0x200;
  static constexpr int FREQUENCY = 432; // audio frequency

  std::array<uint16_t, STACK_SIZE> stack{}; // stores return addresses
  std::array<uint8_t, REGISTER_COUNT> V{};  // registers 0 - F
  std::array<uint8_t, MEMORY_SIZE> memory{};

  uint16_t I = 0;                 // stores memory addresses, use 12 lowest bits
  uint16_t PC = STARTING_ADDRESS; // currently executing address
  uint8_t SP = 0;                 // topmost level of the stack
  uint8_t DT = 0;                 // delay timer register
  uint8_t ST = 0;                 // sound timer register, play if > 0

  /// @brief Jumps to a routine at nnn
  /// Not really used
  /// @param address 0nnn
  void sys(uint16_t address);

  /// @brief Clears the screen
  void cls();

  /// @brief Returns from a subroutine
  void ret();

  /// @brief Jumps to address at nnn
  /// @param address 0nnn
  void jump(uint16_t address);

  /// @brief Calls the routine at nnn
  /// @param address 0nnn
  void call(uint16_t address);

  /// @brief Skips next instruction if V_num == byte.
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void skip_next_if_equal_byte(uint8_t register_num, uint8_t byte);

  /// @brief Skips next instruction if V_x != byte.
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void skip_next_if_not_equal_byte(uint8_t register_num, uint8_t byte);

  /// @brief Skips next instruction if V_x == V_y.
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void skip_next_if_equal_register(uint8_t register_x, uint8_t register_y);

  /// @brief Stores value of byte in V_x.
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void load_from_byte(uint8_t register_num, uint8_t byte);

  /// @brief Adds V_x + byte, and stores in V_x
  /// @param register_num the register number, x in V_x
  /// @param byte the value to compare against
  void add(uint8_t register_num, uint8_t byte);

  /// @brief Stores value of V_y in V_x.
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void load_from_register(uint8_t register_x, uint8_t register_y);

  /// @brief Performs bitwise OR on V_x and V_y, stores in V_x
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_or(uint8_t register_x, uint8_t register_y);

  /// @brief Performs bitwise AND on V_x and V_y, stores in V_x
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_and(uint8_t register_x, uint8_t register_y);

  /// @brief Performs bitwise XOR on V_x and V_y, stores in V_x
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void bitwise_xor(uint8_t register_x, uint8_t register_y);

  /// @brief Stores V_x + V_y in Vx, sets V_f to the carry
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void add_and_store_carry(uint8_t register_x, uint8_t register_y);

  /// @brief Stores V_x - V_y in Vx, sets V_f to not borrow
  /// V_f being 1 implies that the difference is non-negative
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void subtract(uint8_t register_x, uint8_t register_y);

  /// @brief Stores V_x >> 1 in V_x
  /// @param register_num the register_number, x in V_x
  void shift_right(uint8_t register_num);

  /// @brief Stores V_y - V_x in V_x, sets V_f to not borrow
  /// V_f being 1 implies that the difference is non-negative
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void reverse_subtract(uint8_t register_x, uint8_t register_y);

  /// @brief Stores V_x << 1 in V_x
  /// @param register_num the register number, x in V_x
  void shift_left(uint8_t register_num);

  /// @brief Skips next instruction if V_x != V_y.
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  void skip_next_if_not_equal_register(uint8_t register_x, uint8_t register_y);

  /// @brief loads nnn into I
  /// @param address 0nnn
  void load_I(uint16_t address);

  /// @brief Jumps to location nnn + V_x
  /// @param register_num the register number, x in V_x
  /// @param address 0nnn
  void jump_off_register(uint8_t register_num, uint16_t address);

  /// @brief Generates random byte and ANDed with provided byte, stored in V_x
  /// @param register_num the register number, x in V_x
  /// @param byte the value to and with
  void rand(uint8_t register_num, uint8_t byte);

  /// @brief Displays n byte sprite starting at I at (V_x, V_y), V_f = collision.
  /// @param register_x the register number, x in V_x
  /// @param register_y the register number, y in V_y
  /// @param nibble the height of the sprite to draw
  void draw(uint8_t register_x, uint8_t register_y, uint8_t nibble);

  /// @brief Skips the next instruction of key of V_x is pressed
  /// @param register_num the register number, x in V_x
  void skip_if_pressed(uint8_t register_num);

  /// @brief Skips the next instruction of key of V_x is not pressed
  /// @param register_num the register number, x in V_x
  void skip_if_not_pressed(uint8_t register_num);

  /// @brief Loads the value from the delay timer to V_x
  /// @param register_num the register number, x in V_x
  void load_from_delay_timer(uint8_t register_num);

  /// @brief Execution stops until a key is pressed; stored in V_x
  /// @param register_num the register number, x in V_x
  void store_key_press(uint8_t register_num);

  /// @brief Sets the value of the delay timer to V_x
  /// @param register_num the register number, x in V_x
  void set_delay_timer(uint8_t register_num);

  /// @brief Sets the value of the sound timer to V_x
  /// @param register_num the register number, x in V_x
  void set_sound_timer(uint8_t register_num);

  /// @brief Adds I + V_x and stores in the I register
  /// @param register_num the register number, x in V_x
  void add_I(uint8_t register_num);

  /// @brief Sets I to the location of the sprite in V_x
  /// @param register_num the register number, x in V_x
  void load_sprite(uint8_t register_num);

  /// @brief Writes the BCD form of V_x in locations I, I+1, I+2
  /// @param register_num the register number, x in V_x
  void write_binary_coded_decimal(uint8_t register_num);

  /// @brief Stores registers V_0 to V_x into memory, starting at I
  /// @param register_num the register number to stop at, x in V_x
  void store_memory_from_registers(uint8_t register_num);

  /// @brief Stores memory starting at I into V_0 to V_x
  /// @param register_num the register number
  void store_registers_from_memory(uint8_t register_num);
public:
  Chip8();
  explicit Chip8(std::array<uint8_t, MEMORY_SIZE> memory);

  void load_into_memory(std::array<uint8_t, MEMORY_SIZE> memory);
};
