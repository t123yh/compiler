cmake_minimum_required(VERSION 3.17)
project(compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(compiler main.cpp tokenizer.cpp tokenizer.h)