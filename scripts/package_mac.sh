#!/bin/bash

# Exit on error
set -e

# Check if we have a built app
APP_PATH="../build/mac/bin/Rapid.app"
if [ ! -d "$APP_PATH" ]; then
    echo "Error: Application not found at $APP_PATH. Please build the project first."
    exit 1
fi

# Create packages directory
PACKAGE_DIR="../packages/mac"
mkdir -p "$PACKAGE_DIR"

# Get version from CMakeLists.txt
VERSION=$(grep -m1 "VERSION" ../qt_web_editor/CMakeLists.txt | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)

# Create DMG
DMG_NAME="Rapid-${VERSION}-macOS.dmg"
TEMP_DMG="temp.dmg"
VOLUME_NAME="Rapid $VERSION"

# Create a temporary directory for the DMG contents
DMG_TEMP_DIR="dmg_temp"
mkdir -p "$DMG_TEMP_DIR"

# Copy the app to the temporary directory
cp -R "$APP_PATH" "$DMG_TEMP_DIR/"

# Create symbolic link to Applications
ln -s "/Applications" "$DMG_TEMP_DIR/Applications"

# Calculate size
SIZE=$(du -sh "$DMG_TEMP_DIR" | cut -f1)
SIZE=$((${SIZE//[!0-9]} + 10))

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
rm -f "$TEMP_DMG"

echo "Package created: $PACKAGE_DIR/$DMG_NAME"
