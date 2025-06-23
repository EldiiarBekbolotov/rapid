#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$REPO_ROOT/build/ubuntu-latest"
PACKAGE_DIR="$REPO_ROOT/packages/linux"

APP_BINARY="$BUILD_DIR/bin/Rapid"
if [ ! -f "$APP_BINARY" ]; then
  echo "❌ Binary not found at $APP_BINARY"
  exit 1
fi

# Create package directory
mkdir -p "$PACKAGE_DIR"

# Get version from CMakeLists.txt
VERSION=$(grep -m1 "VERSION" "$REPO_ROOT/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
TARBALL_NAME="Rapid-${VERSION}-Linux-x86_64.tar.gz"
TEMP_DIR="$(mktemp -d)"

# Create directory structure
mkdir -p "$TEMP_DIR/Rapid/bin"

# Copy binary and create launcher
cp "$APP_BINARY" "$TEMP_DIR/Rapid/bin/"

# Create desktop entry
cat > "$TEMP_DIR/Rapid/Rapid.desktop" <<EOL
[Desktop Entry]
Version=1.0
Type=Application
Name=Rapid
Comment=Modern Web Editor
Exec=/usr/local/bin/rapid
Icon=rapid
Terminal=false
Categories=Development;
EOL

# Create install script
cat > "$TEMP_DIR/Rapid/install.sh" <<'EOL'
#!/bin/bash
set -e

if [ "$(id -u)" -ne 0 ]; then
  echo "This script must be run as root"
  exit 1
fi

INSTALL_DIR="/opt/rapid"
BIN_DIR="/usr/local/bin"
ICON_DIR="/usr/share/icons/hicolor/256x256/apps"
APPS_DIR="/usr/share/applications"

# Create directories
mkdir -p "$INSTALL_DIR" "$ICON_DIR" "$APPS_DIR"

# Copy files
cp -r bin "$INSTALL_DIR/"
chmod +x "$INSTALL_DIR/bin/"*

# Create symlink
ln -sf "$INSTALL_DIR/bin/Rapid" "$BIN_DIR/rapid"

# Install desktop entry
cp Rapid.desktop "$APPS_DIR/"
chmod 644 "$APPS_DIR/Rapid.desktop"

# Install icon (if available)
if [ -f "share/icons/Rapid.png" ]; then
  cp share/icons/Rapid.png "$ICON_DIR/"
fi

echo "Rapid has been installed to $INSTALL_DIR"
EOL

# Make install script executable
chmod +x "$TEMP_DIR/Rapid/install.sh"

# Create tarball
echo "📦 Creating tarball..."
tar -czf "$PACKAGE_DIR/$TARBALL_NAME" -C "$TEMP_DIR" Rapid

# Clean up
rm -rf "$TEMP_DIR"

echo "✅ Linux package created at: $PACKAGE_DIR/$TARBALL_NAME"
