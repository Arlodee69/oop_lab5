cmake_minimum_required(VERSION 3.15)
project(LAB5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

enable_testing()

find_package(GTest REQUIRED)

add_executable(tests tests/test.cpp)
target_link_libraries(tests GTest::GTest GTest::Main)

add_test(NAME run_tests COMMAND tests)

add_custom_target(check COMMAND tests)