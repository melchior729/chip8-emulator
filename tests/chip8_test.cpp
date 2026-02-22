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
  }
};

// loads the font data into the memory, tests the configured memory
TEST_F(Chip8Test, LoadFontDataWorks) {
  // A is at 5 * 10 = 50 = 0x32, 5 registers since 5 bytes
  load(Chip8::START, 0xA0, 0x32);
  load(Chip8::START + 2, 0xF4, 0x65);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  /// Sprite A is 0xF0, 0x90, 0xF0, 0x90, 0x90
  EXPECT_EQ(cpu.get_register(0), 0xF0);
  EXPECT_EQ(cpu.get_register(1), 0x90);
  EXPECT_EQ(cpu.get_register(2), 0xF0);
  EXPECT_EQ(cpu.get_register(3), 0x90);
  EXPECT_EQ(cpu.get_register(4), 0x90);
}

// sys sets the PC to 0xFFF
TEST_F(Chip8Test, SysSetsPCToAddress) {
  load(Chip8::START, 0x0F, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();
  EXPECT_EQ(cpu.get_PC(), 0xFFF);
}

// cls clears the display buffer
TEST_F(Chip8Test, ClsClearsDisplayBuffer) {
  load(Chip8::START, 0x00, 0xE0);
  cpu.load_into_memory(memory);
  cpu.cycle();

  const auto &display_buffer = cpu.get_display_buffer();
  bool all_zeros = std::all_of(display_buffer.begin(), display_buffer.end(),
                               [](uint8_t x) { return x == 0; });

  EXPECT_TRUE(all_zeros);
}

// ret comes back from a subroutine
TEST_F(Chip8Test, RetReturnsFromSubroutine) {
  // at 0x200, go to 0xF00, put 0x200 on stack
  load(Chip8::START, 0x2F, 0x00);
  // at 0xF00, return call to previous
  load(0xF00, 0x00, 0xEE);
  cpu.load_into_memory(memory);
  cpu.cycle(); // call
  uint8_t sp_after_call = cpu.get_SP();
  cpu.cycle(); // ret
  EXPECT_EQ(cpu.get_PC(), Chip8::START + 2);
  EXPECT_EQ(cpu.get_SP(), sp_after_call - 1);
}

// jumps to an address and checks if its right
TEST_F(Chip8Test, JumpsToCorrectAddress) {
  load(Chip8::START, 0x1F, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();
  EXPECT_EQ(cpu.get_PC(), 0xFFF);
}

// call adds the address to the stack and updates the program counter
TEST_F(Chip8Test, CallAddsToStackAndChangesPC) {
  load(Chip8::START, 0x2F, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), 0xFFF);
  EXPECT_EQ(cpu.get_SP(), 1);
  EXPECT_EQ(cpu.get_stack()[cpu.get_SP() - 1], Chip8::START + 2);
}

// skips the next instruction of the content of register and byte are equal
TEST_F(Chip8Test, SkipNextIfEqualByteWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x30, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 6);
}

// skips the next instruction of the content of register and byte are not equal
TEST_F(Chip8Test, SkipNextIfNotEqualByteWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x40, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 4);
}

// skips the next instruction if the two registers hold the same values
TEST_F(Chip8Test, SkipNextIfEqualRegistersWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x61, 0xFF);
  load(Chip8::START + 4, 0x50, 0x10);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 8);
}

// loads a byte to a given register
TEST_F(Chip8Test, LoadsFromByteToRegisterWorks) {
  load(Chip8::START, 0x60, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();
  EXPECT_EQ(cpu.get_register(0), 0xFF);
}

// adds the content of a register to a byte, and stores in the same register
TEST_F(Chip8Test, AddsRegisterToByteAndStoresInRegister) {
  load(Chip8::START, 0x60, 0x02);
  load(Chip8::START + 2, 0x70, 0x10);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0x12);
}

// copies the value in 1 register to the other
TEST_F(Chip8Test, LoadsRegisterToRegisterWorks) {
  load(Chip8::START, 0x61, 0xFF);
  load(Chip8::START + 2, 0x80, 0x10);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0xFF);
}

// performs bitwise or on V_x and V_y and stores in V_x
TEST_F(Chip8Test, BitWiseOrWorks) {
  load(Chip8::START, 0x60, 0xDA);
  load(Chip8::START + 2, 0x61, 0x2C);
  load(Chip8::START + 4, 0x80, 0x11);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0xFE);
}

// performs bitwise and on V_x and V_y and stores in V_x
TEST_F(Chip8Test, BitWiseAndWorks) {
  load(Chip8::START, 0x60, 0xFB);
  load(Chip8::START + 2, 0x61, 0x2D);
  load(Chip8::START + 4, 0x80, 0x12);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x29);
}

// performs bitwise xor on V_x and V_y and stores in V_x
TEST_F(Chip8Test, BitWiseXorWorks) {
  load(Chip8::START, 0x60, 0xFB);
  load(Chip8::START + 2, 0x61, 0x2D);
  load(Chip8::START + 4, 0x80, 0x13);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0xD6);
}

// adds V_x + V_y in V_x and sets V_f to the carry
TEST_F(Chip8Test, AddsAndStoresCarryInVf) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x61, 0x02);
  load(Chip8::START + 4, 0x80, 0x14);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x01);
  EXPECT_EQ(cpu.get_register(0xF), 0x01);
}

// subtracts V_x - V_y in V_x and sets V_f to not borrow
TEST_F(Chip8Test, SubtractsAndStoresNotBorrowInVf) {
  load(Chip8::START, 0x60, 0x01);
  load(Chip8::START + 2, 0x61, 0xFF);
  load(Chip8::START + 4, 0x80, 0x15);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x02);
  EXPECT_EQ(cpu.get_register(0xF), 0x00);
}

// shifts a register to the right
TEST_F(Chip8Test, ShiftsRegisterToTheRight) {
  load(Chip8::START, 0x60, 0xAD);
  load(Chip8::START + 2, 0x80, 0x06);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0x56);
  EXPECT_EQ(cpu.get_register(0xF), 0x01);
}

// subtracts V_y - V_x in V_x, sets V_f to not borrow
TEST_F(Chip8Test, ReverseSubtractWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x61, 0x01);
  load(Chip8::START + 4, 0x80, 0x17);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x02);
  EXPECT_EQ(cpu.get_register(0xF), 0x00);
}

// shifts a register to the left
TEST_F(Chip8Test, ShiftsRegisterToTheLeft) {
  load(Chip8::START, 0x60, 0xAD);
  load(Chip8::START + 2, 0x80, 0x0E);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0x5A);
  EXPECT_EQ(cpu.get_register(0xF), 0x01);
}

// skips the next instruction of the content of both registers are not equal
TEST_F(Chip8Test, SkipNextIfNotEqualRegistersWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0x61, 0x01);
  load(Chip8::START + 4, 0x90, 0x10);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 8);
}

// loads the address into the I register
TEST_F(Chip8Test, LoadsIFromAddressWorks) {
  load(Chip8::START, 0xAF, 0xFF);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_I(), 0xFFF);
}

// jumps to location nnn + V_0
TEST_F(Chip8Test, JumpsOffRegistersWorks) {
  load(Chip8::START, 0x60, 0x20);
  load(Chip8::START + 2, 0xBF, 0x00);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), 0xF20);
}

// generates a random number and ANDs it with the provided byte
TEST_F(Chip8Test, RandomNumberAndByteWorks) {
  load(Chip8::START, 0xC0, 0x0F);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_LE(cpu.get_register(0), 0x0F);
}

// draws an n byte sprite starting at (V_x, V_y), will wrap around
TEST_F(Chip8Test, DrawUpdatesDisplayBufferProperly) {
  load(Chip8::START, 0x60, 0x3F);
  load(Chip8::START + 2, 0x61, 0x1F);
  load(Chip8::START + 4, 0x62, 0x0A);
  load(Chip8::START + 6, 0xF2, 0x29);
  load(Chip8::START + 8, 0xD0, 0x15);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 5; i++) {
    cpu.cycle();
  }

  std::array<uint8_t, Chip8::WIDTH * Chip8::HEIGHT> expected{};

  /// Sprite A is 0xF0, 0x90, 0xF0, 0x90, 0x90
  expected[31 * 64 + 0] = 1;
  expected[31 * 64 + 1] = 1;
  expected[31 * 64 + 2] = 1;
  expected[31 * 64 + 63] = 1;

  expected[0 * 64 + 2] = 1;
  expected[0 * 64 + 63] = 1;

  expected[1 * 64 + 0] = 1;
  expected[1 * 64 + 1] = 1;
  expected[1 * 64 + 2] = 1;
  expected[1 * 64 + 63] = 1;

  expected[2 * 64 + 2] = 1;
  expected[2 * 64 + 63] = 1;

  expected[3 * 64 + 2] = 1;
  expected[3 * 64 + 63] = 1;

  EXPECT_EQ(expected, cpu.get_display_buffer());
}

// draws the same sprite on top of itself, should all be empty, and vf = 1
TEST_F(Chip8Test, DrawDetectsCollisonAndMarksVf) {
  load(Chip8::START, 0x60, 0x3F);
  load(Chip8::START + 2, 0x61, 0x1F);
  load(Chip8::START + 4, 0x62, 0x0A);
  load(Chip8::START + 6, 0xF2, 0x29);
  load(Chip8::START + 8, 0xD0, 0x15);
  load(Chip8::START + 10, 0xD0, 0x15);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 6; i++) {
    cpu.cycle();
  }

  auto const &display_buffer = cpu.get_display_buffer();
  bool all_zeros = std::all_of(display_buffer.begin(), display_buffer.end(),
                               [](uint8_t x) { return x == 0; });

  EXPECT_TRUE(all_zeros);
  EXPECT_EQ(cpu.get_register(0xF), 1);
}

// skips the next instruction if the key is pressed
TEST_F(Chip8Test, SkipIfPressedKeyWorks) {
  cpu.set_keypad(0, true);
  load(Chip8::START, 0xE0, 0x9E);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 4);
}

// skips the next instruction if the key is not pressed
TEST_F(Chip8Test, SkipIfNotPressedKeyWorks) {
  cpu.set_keypad(0, false);
  load(Chip8::START, 0xE0, 0xA1);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), Chip8::START + 4);
}

// loads the value from the delay timer register to another register
TEST_F(Chip8Test, LoadFromDelayTimerToRegisterWorks) {
  cpu.set_DT(5);
  load(Chip8::START, 0xF0, 0x07);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0x05);
}

// stops the program until a button is pressed, stores it in a register
TEST_F(Chip8Test, ExecutionStopsUntilKeyPressed) {
  load(Chip8::START, 0xF0, 0x0A);
  cpu.load_into_memory(memory);
  cpu.cycle();

  EXPECT_EQ(cpu.get_PC(), Chip8::START);
  cpu.set_keypad(0xF, 1);
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0xF);
  EXPECT_EQ(cpu.get_PC(), Chip8::START + 2);
}

TEST_F(Chip8Test, SetDelayTimerFromRegisterWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0xF0, 0x15);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_DT(), 0xFF);
}

// sets the sound timer register to the provided register
TEST_F(Chip8Test, SetSoundTimerFromRegisterWorks) {
  load(Chip8::START, 0x60, 0xFF);
  load(Chip8::START + 2, 0xF0, 0x18);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_ST(), 0xFF);
}

// adds I + V_x and store it in the I register
TEST_F(Chip8Test, AddIAndRegisterAndStoreInI) {
  load(Chip8::START, 0xAA, 0xBA);
  load(Chip8::START + 2, 0x60, 0x02);
  load(Chip8::START + 4, 0xF0, 0x1E);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 3; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_I(), 0xABC);
}

// sets the value of I to the address of sprite in memory, determined by V_x
TEST_F(Chip8Test, SetsIToSpriteAddress) {
  load(Chip8::START, 0x60, 0x0A);
  load(Chip8::START + 2, 0xF0, 0x29);
  cpu.load_into_memory(memory);
  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_I(), 0x32);
}

// writes the the first decimal digit of V_x in I, then the second in I + 1, and
// the third in I + 2
TEST_F(Chip8Test, WriteBinaryCodedDecimalAtILocationFromRegister) {
  load(Chip8::START, 0x60, 0xF1); // 241
  load(Chip8::START + 2, 0xA3, 0x00);
  load(Chip8::START + 4, 0xF0, 0x33);
  load(Chip8::START + 6, 0xF2, 0x65);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 4; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x02);
  EXPECT_EQ(cpu.get_register(1), 0x04);
  EXPECT_EQ(cpu.get_register(2), 0x01);
}

// stores registers V_0 to V_x, starting at address in I
TEST_F(Chip8Test, StoreRegistersIntoMemoryWorks) {
  load(Chip8::START, 0xAA, 0xBA);
  load(Chip8::START + 2, 0x60, 0x32);
  load(Chip8::START + 4, 0x61, 0x14);
  load(Chip8::START + 6, 0xF1, 0x55);
  load(Chip8::START + 8, 0x60, 0x00);
  load(Chip8::START + 10, 0x61, 0x00);
  load(Chip8::START + 12, 0xF1, 0x65);
  cpu.load_into_memory(memory);

  for (uint8_t i = 0; i < 7; i++) {
    cpu.cycle();
  }

  EXPECT_EQ(cpu.get_register(0), 0x32);
  EXPECT_EQ(cpu.get_register(1), 0x14);
}

// stores memory into V_0 to V_x, starting at address I
TEST_F(Chip8Test, StoreMemoryIntoRegistersWorks) {
  load(Chip8::START, 0xAA, 0xBA);
  load(Chip8::START + 2, 0xF1, 0x65);

  memory[0xABA] = 0x32;
  memory[0xABB] = 0x14;
  cpu.load_into_memory(memory);

  cpu.cycle();
  cpu.cycle();

  EXPECT_EQ(cpu.get_register(0), 0x32);
  EXPECT_EQ(cpu.get_register(1), 0x14);
}
