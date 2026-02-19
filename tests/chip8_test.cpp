/// @file chip8_test.cpp
/// @brief Tetss for the Chip8 class
/// @author Abhay Manoj
/// @date Feb 19 2026

#include "../src/chip8.hpp"
#include <gtest/gtest.h>

class Chip8Test : public ::testing::Test {
protected:
  Chip8 cpu;

  void SetUp() override {
    //
    //
  }

  void TearDown() override {
    //
    //
  }
};

TEST_F(Chip8Test, SysWorks) { cpu. }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
