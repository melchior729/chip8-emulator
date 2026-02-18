find_package(GTest REQUIRED)

add_executable(run_tests chip8_test.cpp)

target_link_libraries(
  run_tests
  PRIVATE
  GTest::gtest_main
  chip8lib
)
