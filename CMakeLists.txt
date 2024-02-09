cmake_minimum_required(VERSION 3.12)

project(circular_buffer)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(addons)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)