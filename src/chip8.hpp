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
public:
  Chip8();
  Chip8(std::array<uint8_t, MEMORY_SIZE> memory);

  void load_into_memory(std::array<uint8_t, MEMORY_SIZE> memory);
};
