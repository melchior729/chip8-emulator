/// @file chip8_test.cpp
/// @brief Tests for the Chip8 class
/// @author Abhay Manoj
/// @date Feb 19 2026

#include "../src/chip8.hpp"
#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>

class Chip8Test : public ::testing::Test {
protected:
  Chip8 cpu;
  std::array<uint8_t, Chip8::MEMORY_SIZE> memory{};

  /// @brief clears memory to all 0s
  void SetUp() override { memory.fill(0); }

  /// @brief loads a command into the memory
  /// @param address the address to load the bytes to
  /// @param byte_x the first byte of the command
  /// @param byte_y the second byte of the command
  void load(uint16_t address, uint8_t byte_x, uint8_t byte_y) {
    memory[address] = byte_x;
    memory[address + 1] = byte_y;
    cpu.load_into_memory(memory);
  }
};

// sys sets the PC to 0xFFF
TEST_F(Chip8Test, SysSetsPCToAddress) {
  load(Chip8::START, 0x0F, 0xFF);
  cpu.cycle();
  EXPECT_EQ(cpu.get_PC(), 0xFFF);
}

// cls clears the display buffer
TEST_F(Chip8Test, ClsClearsDisplayBuffer) {
  load(Chip8::START, 0x00, 0xE0);
  cpu.cycle();

  const auto &display_buffer = cpu.get_display_buffer();
  bool all_zeros = std::all_of(display_buffer.begin(), display_buffer.end(),
                               [](uint8_t x) { return x == 0; });

  EXPECT_TRUE(all_zeros);
}

// ret comes back from a subroutine
TEST_F(Chip8Test, RetReturnsFromSubroutine) {
  load(Chip8::START, 0x2F, 0xFF);
  load(0xFFF, 0x00, 0xEE);
  cpu.cycle(); // call
  uint8_t sp_after_call = cpu.get_SP();
  cpu.cycle(); // ret
  EXPECT_EQ(cpu.get_PC(), Chip8::START + 2);
  EXPECT_EQ(cpu.get_SP(), sp_after_call - 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
