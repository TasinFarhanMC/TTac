cmake_minimum_required(VERSION 3.16)
project(ttac_raylib)

set(CMAKE_CXX_STANDARD 23)

find_package(raylib REQUIRED)

add_executable(ttac_raylib raylib.cpp ../src/ttac.c ../include/ttac.h)
target_include_directories(ttac_raylib PRIVATE ../include/)

target_link_libraries(ttac_raylib raylib)
