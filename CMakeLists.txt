cmake_minimum_required(VERSION 3.1)
project(sarp VERSION 0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# For debug only
add_definitions("-Wall -Wpedantic -Wextra -Wpointer-arith -Wcast-qual -g")

add_subdirectory(src)
