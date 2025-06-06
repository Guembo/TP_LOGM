cmake_minimum_required(VERSION 3.31)
project(LOGM_TP C)

set(CMAKE_C_STANDARD 99)
include_directories(include)

add_executable(LOGM_TP src/main.c
        include/resolution.h
        src/resolution.c
)

