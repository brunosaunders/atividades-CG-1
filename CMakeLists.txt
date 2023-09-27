cmake_minimum_required(VERSION 3.26)
project(creto)

set(CMAKE_CXX_STANDARD 14)

include_directories(include ../SDL/include)

add_executable(creto
        include/Color.hpp
        color.cpp
        main.cpp)
add_compile_options(sdl2-config -cflags -libs)