# Windows Packaging Script (MSI)
$ErrorActionPreference = "Stop"

if (-not (Get-Command "candle.exe" -ErrorAction SilentlyContinue)) {
    Write-Error "❌ WiX Toolset is required. Install it from https://wixtoolset.org/"
    exit 1
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $scriptDir
$buildDir = Join-Path $repoRoot "..\build\windows-latest"
$packageDir = Join-Path $repoRoot "..\packages\windows"
$appPath = Join-Path $buildDir "bin\Release\Rapid.exe"

if (-not (Test-Path $appPath)) {
    Write-Error "❌ App not found at $appPath"
    exit 1
}

$version = (Select-String -Path "$repoRoot\CMakeLists.txt" -Pattern 'VERSION ([0-9]+\.[0-9]+\.[0-9]+)').Matches.Groups[1].Value
$tempDir = "$env:TEMP\RapidInstaller"
$installDir = "$tempDir\Rapid"
New-Item -ItemType Directory -Force -Path $installDir | Out-Null
Copy-Item "$buildDir\bin\Release\*" -Destination $installDir -Recurse -Force
New-Item -ItemType Directory -Force -Path $packageDir | Out-Null

$wxs = @"
<?xml version='1.0'?>
<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>
  <Product Name='Rapid' Id='*' UpgradeCode='12345678-1234-1234-1234-1234567890ab' Version='$version' Manufacturer='Rapid'>
    <Package InstallerVersion='200' Compressed='yes' InstallScope='perMachine' />
    <Directory Id='TARGETDIR' Name='SourceDir'>
      <Directory Id='ProgramFilesFolder'>
        <Directory Id='INSTALLFOLDER' Name='Rapid'>
          <Component Id='MainEXE' Guid='*'>
            <File Id='RapidEXE' Name='Rapid.exe' Source='$installDir\Rapid.exe' KeyPath='yes' />
          </Component>
        </Directory>
      </Directory>
    </Directory>
    <Feature Id='Main' Title='Rapid App' Level='1'>
      <ComponentRef Id='MainEXE' />
    </Feature>
  </Product>
</Wix>
"@
$wxsPath = "$tempDir\Rapid.wxs"
$wxs | Out-File -Encoding utf8 -FilePath $wxsPath

& candle.exe -out "$tempDir\Rapid.wixobj" "$wxsPath"
& light.exe -out "$packageDir\Rapid-${version}-Windows.msi" "$tempDir\Rapid.wixobj"

Remove-Item -Recurse -Force $tempDir
Write-Host "✅ Windows MSI created at: $packageDir\Rapid-${version}-Windows.msi"
