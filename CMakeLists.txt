cmake_minimum_required(VERSION 3.10)
project(function)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,leak")

add_executable(function testing.cpp function.hpp)