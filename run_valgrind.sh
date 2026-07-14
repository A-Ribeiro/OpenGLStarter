#!/bin/sh
#
# CMake Config to Use Valgrind on Debug
#
# add_compile_options(-g -O0 -fno-inline -fno-omit-frame-pointer)
# SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
#
# or
#
# target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -fno-inline -fno-omit-frame-pointer)
# target_link_options(${PROJECT_NAME} PRIVATE -no-pie)
#

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         "$@"
