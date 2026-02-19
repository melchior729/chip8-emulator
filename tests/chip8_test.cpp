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

TEST(Chip8Test, )

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
