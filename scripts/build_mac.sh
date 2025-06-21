#!/bin/bash

# Exit on error
set -e

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/build/macos"

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
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$BUILD_DIR/bin" \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="$BUILD_DIR/lib" \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="$BUILD_DIR/lib" \
    "$REPO_ROOT/qt_web_editor"

# Build the project
echo "Building project..."
cmake --build . --config Release -- -j$(sysctl -n hw.logicalcpu)

# Verify the app was built
APP_PATH="$BUILD_DIR/bin/Rapid.app"
if [ ! -d "$APP_PATH" ]; then
    echo "Error: Application not found at $APP_PATH"
    echo "Build may have completed but the app bundle was not created in the expected location."
    echo "Checking for app in build directory..."
    find "$BUILD_DIR" -name "*.app" -type d
    exit 1
fi

echo "Build complete! You can find the application in: $APP_PATH"
