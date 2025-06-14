# Qt Web Editor

A lightweight web development text editor built with Qt, inspired by Brackets.

## Features

- Cross-platform (Windows, macOS, Linux)
- File and folder browser
- Syntax highlighting for HTML, CSS, JavaScript, and more
- Tabbed interface for multiple files
- Real-time preview for HTML files
- Light and dark themes
- Customizable editor settings

## Requirements

- Qt 5.15 or later (or Qt 6.0 or later)
- CMake 3.16 or later
- C++17 compatible compiler

## Building

### Linux/macOS

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

### Windows

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## Running

After building, the executable will be in the `build` directory:

```bash
./QtWebEditor
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Screenshots

*(Screenshots will be added later)*

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
