cmake_minimum_required(VERSION 3.10)
project(TTAC_RAYLIB)

find_package(raylib REQUIRED)

add_executable(ttac_raylib "${CMAKE_CURRENT_SOURCE_DIR}/raylib.cpp")
target_link_libraries(ttac_raylib raylib TTac)
