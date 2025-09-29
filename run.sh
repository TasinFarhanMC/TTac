#!/bin/bash

set -e

DIR="$(dirname "$(realpath "$0")")"
cd "$DIR"

echo "Press 'r' to rerun cmake, 'g' to run gdb, or Enter to build and run gdb:"
read -r -n1 key
echo

if [[ "$key" == "r" ]]; then
  echo "Running cmake configuration..."
  cmake --preset debug -G Ninja
  echo "Building..."
  cmake --build --preset debug
  echo "Running..."
  ./bin/tests
elif [[ "$key" == "g" ]]; then
  echo "Building..."
  cmake --build --preset debug
  echo "Running gdb..."
  gdb ./bin/tests
else
  echo "Building..."
  cmake --build --preset debug
  echo "Running..."
  ./bin/tests
fi
