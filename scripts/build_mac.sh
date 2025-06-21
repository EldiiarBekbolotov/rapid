#!/bin/bash

# Exit on error
set -e

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/build/macos-latest"

# Clean previous build
echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14 \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    "$REPO_ROOT/qt_web_editor"

# Build the project
echo "Building project..."
cmake --build . --config Release -- -j$(sysctl -n hw.logicalcpu)

echo "Build complete! You can find the application in: $(pwd)/bin"
