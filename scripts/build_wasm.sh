#!/bin/bash

# Exit on error
set -e

# Check if Emscripten is available
if ! command -v em++ &> /dev/null; then
    echo "Error: Emscripten (em++) is not in PATH. Please install and activate it first.
See: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/build/ubuntu-latest"

# Clean previous build
echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with Emscripten
echo "Configuring with Emscripten..."
emcmake cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
    -DQT_HOST_PATH=/usr/local/Qt-6.5.0 \
    -DQT_HOST_PATH_CMAKE_DIR=/usr/local/Qt-6.5.0/lib/cmake \
    -DQT_QMAKE_EXECUTABLE=/usr/local/Qt-6.5.0/bin/qmake \
    -DQT_USE_BUNDLED_BUNDLED_Qt6=ON \
    -DQT_FEATURE_webengine_webrtc=OFF \
    -DQT_FEATURE_webengine_webchannel=OFF \
    -DQT_FEATURE_webengine_spellchecker=OFF \
    -DQT_FEATURE_webengine_printing_and_pdf=OFF \
    "$REPO_ROOT/qt_web_editor"

# Build the project
echo "Building project..."
cmake --build . --config Release -- -j$(nproc)

echo "WASM build complete! You can find the output in: $(pwd)/bin"

echo "To test the WASM version, you can use a simple HTTP server:"
echo "  cd $(pwd)/bin"
echo "  python3 -m http.server 8000"
echo "Then open http://localhost:8000 in a web browser"
