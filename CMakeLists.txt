cmake_minimum_required(VERSION 3.31)
project(LOGM_TP C)

set(CMAKE_C_STANDARD 99)
include_directories(include)

add_executable(LOGM_TP
        src/main.c
        src/resolution.c
        src/clauses_avl.c
        include/resolution.h
        include/clauses_avl.h
)

add_executable(sodoku_exec
        src/sodoku_by_resolution.c
        src/resolution.c
        src/clauses_avl.c
        include/sodoku_by_resolution.h
        include/resolution.h
        include/clauses_avl.h
)
#set_target_properties(LOGM_TP PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#)

