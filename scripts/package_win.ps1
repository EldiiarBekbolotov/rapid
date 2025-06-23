# Windows packaging script
$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = "$RepoRoot\build\windows-latest"
$PackageDir = "$RepoRoot\packages\win"
$ExePath = "$BuildDir\Release\Rapid.exe"

if (-not (Test-Path $ExePath)) {
    Write-Error "❌ Executable not found at $ExePath"
    exit 1
}

# Create package directory
New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null

# Get version from CMakeLists.txt
$Version = (Select-String -Path "$RepoRoot\CMakeLists.txt" -Pattern 'VERSION ([0-9.]+)').Matches.Groups[1].Value
$ZipName = "Rapid-${Version}-Windows.zip"
$ZipPath = "$PackageDir\$ZipName"

# Create a temp directory
$TempDir = "$env:TEMP\RapidPackage"
if (Test-Path $TempDir) {
    Remove-Item -Recurse -Force $TempDir
}
New-Item -ItemType Directory -Path $TempDir | Out-Null

# Copy required files
Copy-Item $ExePath $TempDir
Copy-Item "$BuildDir\Release\*.dll" $TempDir -ErrorAction SilentlyContinue

# Create ZIP
Compress-Archive -Path "$TempDir\*" -DestinationPath $ZipPath -Force

# Clean up
Remove-Item -Recurse -Force $TempDir

Write-Host "✅ Windows package created at: $ZipPath"
