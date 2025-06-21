# Rapid

A lightweight web development text editor built with C++ and Qt.

![Screenshot](https://github.com/user-attachments/assets/58efed19-102e-4f39-9857-b778185f80ff)

## Features

- Cross-platform (macOS, Windows, Linux)
- File and folder browser
- Syntax highlighting for HTML, CSS, JavaScript
- Tabbed interface for multiple files
- Real-time preview for HTML files
- Light and dark themes

## Building

### Prerequisites

- Qt 6.5.0 or later
- CMake 3.16 or later
- C++17 compatible compiler
- Python 3.6+ (for build scripts)

### Using the build script

```bash
# Make the script executable
chmod +x scripts/build.sh

# Build the project
./scripts/build.sh
```
   pip install aqtinstall
   aqt install-qt mac desktop 6.5.0 clang_64 -m qtwebengine

### Manual build
   # Make scripts executable
   chmod +x scripts/*.sh
   
   # Build the project
   ./scripts/build.sh
   
   # Create a package
   ./scripts/package.sh
   ```

### Manual Build

1. **Install dependencies**:
   - macOS: `brew install qt@6 cmake`
   - Ubuntu: `sudo apt install qt6-base-dev cmake build-essential`
   - Windows: Install Qt 6.5.0 and CMake

2. **Configure and build**:
   ```bash
   mkdir -p build && cd build
   cmake -DCMAKE_PREFIX_PATH=/path/to/qt/6.5.0/gcc_64 ..
   cmake --build . --config Release
   ```

## GitHub Actions

The project includes GitHub Actions workflows for:
- Building on push/pull requests
- Creating releases on tags
- Cross-platform packaging

To create a new release:
```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Project Structure

- `src/` - Source code
- `resources/` - Icons and other resources
- `forms/` - UI definition files
- `cmake/` - CMake configuration files

## Building for Other Platforms

### Linux

Same as macOS instructions above. Make sure Qt development packages are installed.

### Windows

1. Open "x64 Native Tools Command Prompt for VS 2022"
2. Navigate to the project directory
3. Run:
   ```cmd
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```
4. Run `QtWebEditor-1.0.0.exe` from the build directory

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Author: [Eldiiar Bekbolotov](https://github.com/eldiiarbekbolotov) 
