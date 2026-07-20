find_package(GTest REQUIRED)

add_executable(run_tests chip8_test.cpp ../src/chip8.cpp)

target_include_directories(run_tests PRIVATE ../src)

target_link_libraries(
  run_tests
  PRIVATE
  GTest::gtest_main
)
