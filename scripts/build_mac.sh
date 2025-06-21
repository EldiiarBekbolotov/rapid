#!/bin/bash

# Exit on error
set -e

# Clean previous build
echo "Cleaning previous build..."
rm -rf ../build/mac

# Create build directory
mkdir -p ../build/mac
cd ../build/mac

# Configure with CMake
echo "Configuring with CMake..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14 \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    ../../qt_web_editor

# Build the project
echo "Building project..."
cmake --build . --config Release -- -j$(sysctl -n hw.logicalcpu)

echo "Build complete! You can find the application in: $(pwd)/bin"
