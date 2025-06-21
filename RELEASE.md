# Release Process for Rapid Web Editor

This document outlines the steps to create distributable packages for different platforms.

## Prerequisites

### For All Platforms
- CMake 3.16 or later
- Git
- Qt 6.x development libraries

### For macOS
- Xcode Command Line Tools
- hdiutil (included with macOS)

### For Windows
- Visual Studio 2019 or later with C++ workload
- WiX Toolset (for creating MSI installers)

### For WebAssembly
- Emscripten SDK (https://emscripten.org/docs/getting_started/downloads.html)
- Python 3 (for local testing)

## Building for macOS

1. Make the build script executable:
   ```bash
   chmod +x scripts/build_mac.sh
   chmod +x scripts/package_mac.sh
   ```

2. Build the application:
   ```bash
   ./scripts/build_mac.sh
   ```

3. Create a DMG package:
   ```bash
   ./scripts/package_mac.sh
   ```

   The DMG file will be created in `packages/mac/`.

## Building for Windows

1. Open PowerShell as Administrator

2. Build the application:
   ```powershell
   .\scripts\build_windows.ps1
   ```

3. Create an MSI installer:
   ```powershell
   .\scripts\package_windows.ps1
   ```

   The MSI installer will be created in `packages/windows/`.

## Building for Web (WASM)

1. Install and activate Emscripten:
   ```bash
   # Follow instructions at: https://emscripten.org/docs/getting_started/downloads.html
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install latest
   ./emsdk activate latest
   source ./emsdk_env.sh
   ```

2. Make the build script executable:
   ```bash
   chmod +x scripts/build_wasm.sh
   ```

3. Build the WebAssembly version:
   ```bash
   ./scripts/build_wasm.sh
   ```

4. Test the WebAssembly version locally:
   ```bash
   cd build/wasm/bin
   python3 -m http.server 8000
   ```
   Then open http://localhost:8000 in a web browser.

## Versioning

The version number is automatically extracted from `qt_web_editor/CMakeLists.txt`. Update the `VERSION` variable in that file before creating a new release.

## Release Checklist

- [ ] Update version number in CMakeLists.txt
- [ ] Update CHANGELOG.md (if exists)
- [ ] Test all platforms
  - [ ] macOS build and package
  - [ ] Windows build and package
  - [ ] WebAssembly build and test in browser
- [ ] Create a GitHub release
- [ ] Upload all platform packages to the release
- [ ] Update website/downloads page (if applicable)
- [ ] Announce the release (blog, social media, etc.)
