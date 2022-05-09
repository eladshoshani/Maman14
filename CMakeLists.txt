cmake_minimum_required(VERSION 3.16)
project(Maman14 C)

set(CMAKE_C_STANDARD 90)

add_executable(Maman14 main.c tries.c first_pass.c assembler.h line_errors.c tables.c second_pass.c arguments.c files.c assembler.c)