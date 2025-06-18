# Rapid (qt_web_editor)

A lightweight web development text editor built with C++ and Qt.

## Quick Start (Tested on macOS)

1. **Navigate to the project directory**:
   ```bash
   cd /path/to/rapid/qt_web_editor
   ```

2. **Clean previous build (if any)**:
   ```bash
   rm -rf build
   ```

3. **Create build directory and run CMake**:
   ```bash
   mkdir -p build
   cd build
   cmake ..
   ```

4. **Build the project**:
   ```bash
   cmake --build .
   ```

5. **Run the application**:
   ```bash
   ./QtWebEditor-1.0.0
   ```

## Features

- Cross-platform
- File and folder browser
- Syntax highlighting for HTML, CSS, JavaScript
- Tabbed interface for multiple files
- Real-time preview for HTML files
- Light and dark themes
- and more...

## Prerequisites

- Qt 6.x (tested with Qt 6.5.0)
- CMake 3.16+
- C++17 compatible compiler (tested with Apple Clang 17.0.0)

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