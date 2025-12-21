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
  option(${name} "Enable ${name}" OFF)

  if(${name})
    target_compile_definitions(TTac PUBLIC ${name}=1)
  endif()
endfunction()

add_macro(TTAC_C_RANDOM)
add_macro(TTAC_USER_RANDOM)

if(TTAC_TESTS)
  enable_language(CXX)

  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpm.cmake)
  cpmaddpackage("gh:catchorg/Catch2@3.9.0")

  file(GLOB_RECURSE TEST_FILES "${TEST_DIR}/*.cpp")
  add_executable(tests ${TEST_FILES})
  target_compile_features(tests PRIVATE cxx_std_17)
  target_link_libraries(tests PRIVATE Catch2::Catch2WithMain TTac)
  target_include_directories(tests PRIVATE ${TEST_DIR})

  enable_testing()
  include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
  catch_discover_tests(tests)
endif()

if(TTAC_EXAMPLE)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/example")
endif()
