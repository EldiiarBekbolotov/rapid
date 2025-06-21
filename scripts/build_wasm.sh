#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/../build/ubuntu-latest"

echo "🧹 Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "⚙️ Configuring with Emscripten CMake..."
source /opt/emsdk/emsdk_env.sh
emcmake cmake \
  -DCMAKE_BUILD_TYPE=Release \
  "$REPO_ROOT"

echo "🔨 Building project..."
cmake --build . --config Release -- -j$(nproc)

echo "✅ WASM build done at: $BUILD_DIR/bin"
echo "🌐 Run with: python3 -m http.server 8000"
