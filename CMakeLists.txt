cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(compiler main.cpp tokenizer.cpp parser.cpp parsing_failure.cpp parser_const.cpp parser_program.cpp parser_variable.cpp parser_variable.cpp parser_variable.cpp parser_variable.h defs.h parser_function.cpp parser_function.h parser_statement.cpp parser_statement.h parser_expression.cpp parser_expression.h parser_complex_statement.cpp parser_complex_statement.h errors.cpp symbol_table.cpp defs.cpp)
