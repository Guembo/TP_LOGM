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
        include/formule_to_cnf.h
        src/formule_to_cnf.c
)

add_executable(sodoku_exec
        src/sodoku_by_resolution.c
        src/resolution.c
        src/clauses_avl.c
        include/sodoku_by_resolution.h
        include/resolution.h
        include/clauses_avl.h
)

add_executable(fnc_exec
        src/formule_to_cnf.c
        include/formule_to_cnf.h
        src/resolution.c
        include/resolution.h
        src/clauses_avl.c
        include/clauses_avl.h
        src/fnc_main.c
)
#set_target_properties(LOGM_TP PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#)

