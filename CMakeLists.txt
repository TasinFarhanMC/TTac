cmake_minimum_required(VERSION 3.11)
project(TTAC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

add_library(ttac STATIC "${SRC_FILES}")
target_include_directories(ttac PUBLIC ${INCLUDE_DIRS})

if(TTAC_TESTS)

endif()
