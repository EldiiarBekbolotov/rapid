#!/bin/bash

# Exit on error
set -e

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/build/macos"
PACKAGE_DIR="$REPO_ROOT/packages/mac"

# Ensure build directory exists
mkdir -p "$BUILD_DIR"

# Check if we have a built app
APP_PATH="$BUILD_DIR/bin/Rapid.app"
if [ ! -d "$APP_PATH" ]; then
    echo "Error: Application not found at $APP_PATH"
    echo "Please build the project first by running build_mac.sh"
    exit 1
fi

# Create packages directory
mkdir -p "$PACKAGE_DIR"

# Get version from CMakeLists.txt
VERSION=$(grep -m1 "VERSION" "$REPO_ROOT/qt_web_editor/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)

# Create DMG
DMG_NAME="Rapid-macOS.dmg"
VOLUME_NAME="Rapid $VERSION"

# Create a temporary directory for the DMG contents
DMG_TEMP_DIR="$(mktemp -d)"

# Copy the app to the temporary directory
cp -R "$APP_PATH" "$DMG_TEMP_DIR/"

# Create symbolic link to Applications
ln -s "/Applications" "$DMG_TEMP_DIR/Applications"

# Create the DMG
hdiutil create \
    -volname "$VOLUME_NAME" \
    -srcfolder "$DMG_TEMP_DIR" \
    -ov \
    -format UDZO \
    -fs HFS+ \
    "$PACKAGE_DIR/$DMG_NAME"

# Clean up
rm -rf "$DMG_TEMP_DIR"

echo "Package created: $PACKAGE_DIR/$DMG_NAME"
