cmake_minimum_required(VERSION 3.22)
project(Hue)

set(CMAKE_CXX_STANDARD 20)

add_executable(Hue Main.cpp Lexer/Lexer.cpp Lexer/Lexer.h Parser/Parser.cpp Parser/Parser.h)
