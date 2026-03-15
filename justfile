set shell := ["bash", "-cu"]

alias exm := example
alias exm-gdb := example-gdb

# Cmake configuration
cmake:
    cmake --preset debug -G Ninja

# Build
build:
    @echo "Building..."
    cmake --build --preset debug

# Run tests
run: build
    @echo "Running Tests..."
    ./bin/tests

# Run example
example: build
    @echo "Running Example..."
    ./bin/ttac_raylib

# Run tests with gdb
[group('gdb')]
test-gdb: build
    @echo "Running tests..."
    gdb ./bin/tests

# Run example with gdb
[group('gdb')]
example-gdb: build
    @echo "Building..."
    cmake --build --preset debug
    @echo "Running tests..."
    gdb ./bin/ttac_raylib

# Build web
web:
    @echo "Running Cmake Configuration..."
    emcmake cmake --preset web -G Ninja
    @echo "Building..."
    cmake --build --preset web
