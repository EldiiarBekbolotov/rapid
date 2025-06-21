# Windows build script
# Run this in PowerShell with admin privileges

# Exit on error
$ErrorActionPreference = "Stop"

# Clean previous build
Write-Host "Cleaning previous build..."
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue ..\build\windows

# Create build directory
New-Item -ItemType Directory -Force -Path ..\build\windows | Out-Null
Set-Location ..\build\windows

# Find Visual Studio generator
$generator = "Visual Studio 17 2022"  # Update this to your VS version

# Configure with CMake
Write-Host "Configuring with CMake..."
cmake `
    -G "$generator" `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    ..\..\qt_web_editor

# Build the project
Write-Host "Building project..."
cmake --build . --config Release -- /m:$env:NUMBER_OF_PROCESSORS

Write-Host "Build complete! You can find the application in: $(Get-Location)\bin\Release"
