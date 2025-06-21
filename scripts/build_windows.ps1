# Windows Build Script
$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $scriptDir
$buildDir = Join-Path $repoRoot "..\build\windows-latest"

Write-Host "🧹 Cleaning previous build..."
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir | Out-Null
Set-Location $buildDir

Write-Host "⚙️ Configuring with CMake..."
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release "$repoRoot"

Write-Host "🔨 Building project..."
cmake --build . --config Release -- /m:$env:NUMBER_OF_PROCESSORS

Write-Host "✅ Windows Build complete at: $buildDir\bin\Release"
