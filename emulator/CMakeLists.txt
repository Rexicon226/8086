cmake_minimum_required(VERSION 3.22)
project(8086_emulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(8086_emulator
        src/main.cpp
        src/cpu.cpp
        src/cpu.h
)
