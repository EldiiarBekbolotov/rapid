# Windows build script
# Run this in PowerShell with admin privileges

# Exit on error
$ErrorActionPreference = "Stop"

# Get the directory of this script
$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$repoRoot = Split-Path -Parent $scriptDir
$buildDir = Join-Path $repoRoot "build\windows-latest"

# Clean previous build
Write-Host "Cleaning previous build..."
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force -Path $buildDir -ErrorAction SilentlyContinue
}

# Create build directory
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Set-Location $buildDir

# Find Visual Studio generator
$generator = "Visual Studio 17 2022"  # Update this to your VS version

# Configure with CMake
Write-Host "Configuring with CMake..."
$qtWebEditorPath = Join-Path $repoRoot "qt_web_editor"
cmake `
    -G "$generator" `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    $qtWebEditorPath

# Build the project
Write-Host "Building project..."
cmake --build . --config Release -- /m:$env:NUMBER_OF_PROCESSORS

Write-Host "Build complete! You can find the application in: $(Get-Location)\bin\Release"
