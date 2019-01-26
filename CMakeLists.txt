cmake_minimum_required(VERSION 3.13)
project(function)

set(CMAKE_CXX_STANDARD 17)

add_executable(function testing.cpp function.cpp function.hpp)