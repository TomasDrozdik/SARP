cmake_minimum_required(VERSION 3.1)
project(sarp VERSION 0.1)

# TODO: shouldn't be used replace with language features
set(CMAKE_CXX_STANDARD 17)

# For debug only
if (MSVC)
	add_definitions("/W4")
else()
	add_definitions("-Wall -Wpedantic -Wextra -Wpointer-arith -Wcast-qual")
endif()

add_subdirectory(src)
