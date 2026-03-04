#!/bin/bash

# Simple build script for the City Builder Game

echo "=== Building City Builder Game ==="

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Run CMake
echo "Running CMake..."
cmake .. || { echo "CMake failed!"; exit 1; }

# Build the project
echo "Building project..."
make || { echo "Build failed!"; exit 1; }

echo ""
echo "=== Build successful! ==="
echo "Run the game with: ./build/CityBuilderGame"
echo ""
