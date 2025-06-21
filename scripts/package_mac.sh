#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/../build/macos-latest"
PACKAGE_DIR="$REPO_ROOT/../packages/mac"

APP_PATH="$BUILD_DIR/bin/Rapid.app"
if [ ! -d "$APP_PATH" ]; then
  echo "❌ App not found at $APP_PATH"
  exit 1
fi

mkdir -p "$PACKAGE_DIR"
VERSION=$(grep -m1 "VERSION" "$REPO_ROOT/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')

DMG_NAME="Rapid-${VERSION}-macOS.dmg"
VOLUME_NAME="Rapid $VERSION"
TEMP_DIR="dmg_temp"

rm -rf "$TEMP_DIR"
mkdir -p "$TEMP_DIR"
cp -R "$APP_PATH" "$TEMP_DIR/"
ln -s /Applications "$TEMP_DIR/Applications"

echo "💽 Creating DMG..."
hdiutil create \
  -volname "$VOLUME_NAME" \
  -srcfolder "$TEMP_DIR" \
  -ov \
  -format UDZO \
  "$PACKAGE_DIR/$DMG_NAME"

rm -rf "$TEMP_DIR"
echo "✅ macOS DMG created at: $PACKAGE_DIR/$DMG_NAME"
