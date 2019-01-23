cmake_minimum_required(VERSION 3.13)
project(BP)

set(CMAKE_CXX_STANDARD 14)

add_executable(BP src/main.cpp src/lex_analyzator.cpp lib/lex_analyzator.h lib/global.h)