cmake_minimum_required(VERSION 3.10)
project(TTAC_RAYLIB C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================================================
# Detect Emscripten
# ======================================================
if(EMSCRIPTEN)
  message(STATUS "Emscripten detected: building for WebAssembly")
  set(PLATFORM Web)
endif()

# ======================================================
# Include CPM (fetch raylib)
# ======================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/get_cpm.cmake)
cpmaddpackage("gh:raysan5/raylib#5.5")

# ======================================================
# Add executable
# ======================================================
add_executable(ttac_raylib "${CMAKE_CURRENT_SOURCE_DIR}/raylib.cpp"
                           "${CMAKE_CURRENT_SOURCE_DIR}/../src/ttac.c")

# Link raylib
target_link_libraries(ttac_raylib PRIVATE raylib)

# Include project headers
target_include_directories(ttac_raylib
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/")

# ======================================================
# Emscripten-specific target properties
# ======================================================
if(EMSCRIPTEN)
  # Recommended flags for async + WebGL
  set_target_properties(
    ttac_raylib
    PROPERTIES
      LINK_FLAGS
      "-s USE_GLFW=3 -s FULL_ES2=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s ASYNCIFY=1"
  )
endif()
