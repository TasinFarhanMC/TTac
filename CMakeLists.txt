cmake_minimum_required(VERSION 3.11)
project(TTAC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

add_library(TTac STATIC "${SRC_DIR}/ttac.c")
target_include_directories(TTac PUBLIC ${INCLUDE_DIRS})

function(add_macro name)
  if(DEFINED ${name})
    set(value "${${name}}")
    target_compile_definitions(TTac PUBLIC "${name}=${value}")
  endif()
endfunction()

add_macro(TTAC_C_RANDOM)
add_macro(TTAC_USER_RANDOM)

if(TTAC_TESTS)
  enable_language(CXX)

  file(GLOB_RECURSE TEST_FILES "${TEST_DIR}/*.cpp")
  add_executable(tests ${TEST_FILES})
  target_compile_features(tests PRIVATE cxx_std_17)
  target_link_libraries(tests PRIVATE Catch2::Catch2WithMain TTac)
  target_include_directories(tests PRIVATE ${TEST_DIR})

  if(${CPM_USE_LOCAL_PACKAGES})
    find_package(Catch2 3 REQUIRED QUIET)
  endif()

  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpm.cmake)
  if(NOT ${Catch2_FOUND})
    cpmaddpackage("gh:catchorg/Catch2@3.9.0")
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
  endif()

  enable_testing()
  include(Catch)
  catch_discover_tests(tests)
endif()

if(TTAC_EXAMPLE)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/example")
endif()
