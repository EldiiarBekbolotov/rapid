#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/../build/macos-latest"

echo "🧹 Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "⚙️ Configuring with CMake..."
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14 \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
  "$REPO_ROOT"

echo "🔨 Building project..."
cmake --build . --config Release --parallel $(sysctl -n hw.logicalcpu)

echo "✅ macOS Build complete at: $BUILD_DIR/bin"
