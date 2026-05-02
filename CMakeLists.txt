cmake_minimum_required(VERSION 3.12)

project(risc-v-emulator
        VERSION 0.1
        DESCRIPTION "A RISC-V emulator written in C++"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)