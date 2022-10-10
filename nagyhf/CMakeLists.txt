cmake_minimum_required(VERSION 3.23)
project(nagyhf C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(nagyhf
        main.c
        mnist.h)
