cmake_minimum_required(VERSION 3.12)
project(ft_regex C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
file(GLOB SOURCES
        src/*.c)

add_library(ft_regex ${SOURCES} src/group.c inc/ft_regex_defines.h inc/ft_regex_types.h inc/ft_regex_functions.h)
