cmake_minimum_required(VERSION 3.1)
project(sarp VERSION 0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# TODO: shouldn't be used replace with language features
set(CMAKE_CXX_STANDARD 17)

# For debug only
add_definitions("-Wall -Wpedantic -Wextra -Wpointer-arith -Wcast-qual")

add_subdirectory(src)
