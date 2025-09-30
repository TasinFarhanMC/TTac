cmake_minimum_required(VERSION 3.11)
project(TTAC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

add_library(TTac STATIC "${SRC_FILES}")
target_include_directories(TTac PUBLIC ${INCLUDE_DIRS})

if(TTAC_TESTS)
  enable_language(CXX)

  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpm.cmake)
  cpmaddpackage("gh:catchorg/Catch2@3.9.0")

  file(GLOB TEST_FILES "${TEST_DIR}/*.cpp")
  add_executable(tests ${TEST_FILES})
  target_compile_features(tests PRIVATE cxx_std_17)
  target_link_libraries(tests PRIVATE Catch2::Catch2WithMain TTac)

  enable_testing()
  include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
  catch_discover_tests(tests)
endif()
