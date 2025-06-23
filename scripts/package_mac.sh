#!/bin/bash
set -euo pipefail

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$REPO_ROOT/../build/macos-latest}"
PACKAGE_DIR="${PACKAGE_DIR:-$REPO_ROOT/../packages/mac}"
APP_NAME="Rapid"

# Check for required tools
for cmd in hdiutil; do
    if ! command -v $cmd >/dev/null 2>&1; then
        echo -e "${RED}❌ Error: $cmd is required but not installed${NC}" >&2
        exit 1
    fi
done

# Ensure directories exist
mkdir -p "$PACKAGE_DIR"

# Get version from CMakeLists.txt
VERSION=$(grep -m1 "VERSION" "$REPO_ROOT/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo "1.0.0")

# Define paths
APP_PATH="$BUILD_DIR/bin/$APP_NAME.app"
DMG_NAME="${APP_NAME}-${VERSION}-macOS.dmg"
VOLUME_NAME="$APP_NAME $VERSION"
TEMP_DIR="$(mktemp -d)"

# Verify app exists
if [ ! -d "$APP_PATH" ]; then
    echo -e "${RED}❌ Error: $APP_NAME.app not found at $APP_PATH${NC}" >&2
    exit 1
fi

echo -e "${GREEN}📦 Preparing $APP_NAME $VERSION package...${NC}"

# Copy app to temp directory
echo "📂 Setting up package contents..."
cp -a "$APP_PATH" "$TEMP_DIR/"
ln -s "/Applications" "$TEMP_DIR/Applications"

# Create DMG
echo "💽 Creating DMG..."
set +e
hdiutil create \
    -volname "$VOLUME_NAME" \
    -srcfolder "$TEMP_DIR" \
    -ov \
    -format UDZO \
    -fs HFS+ \
    -imagekey zlib-level=9 \
    "$PACKAGE_DIR/$DMG_NAME"

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Failed to create DMG${NC}" >&2
    rm -rf "$TEMP_DIR"
    exit 1
fi
set -e

# Clean up
rm -rf "$TEMP_DIR"

# Verify DMG was created
if [ ! -f "$PACKAGE_DIR/$DMG_NAME" ]; then
    echo -e "${RED}❌ Error: Failed to create DMG${NC}" >&2
    exit 1
fi

# Print success message
DMG_SIZE=$(du -sh "$PACKAGE_DIR/$DMG_NAME" | cut -f1)
echo -e "${GREEN}✅ Successfully created $DMG_NAME (${DMG_SIZE})${NC}"
echo -e "   Path: ${PACKAGE_DIR}/$DMG_NAME"

exit 0
