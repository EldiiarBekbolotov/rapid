# Windows packaging script
# Requires WiX Toolset (https://wixtoolset.org/)

# Exit on error
$ErrorActionPreference = "Stop"

# Check if WiX is installed
if (-not (Get-Command "candle.exe" -ErrorAction SilentlyContinue)) {
    Write-Error "WiX Toolset is required. Please install it from https://wixtoolset.org/"
    exit 1
}

# Get the directory of this script
$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$repoRoot = Split-Path -Parent $scriptDir
$buildDir = Join-Path $repoRoot "build\windows-latest"
$packageDir = Join-Path $repoRoot "packages\windows"

# Check if we have a built app
$appPath = Join-Path $buildDir "bin\Release\Rapid.exe"
if (-not (Test-Path $appPath)) {
    Write-Error "Error: Application not found at $appPath. Please build the project first."
    exit 1
}

# Create packages directory
New-Item -ItemType Directory -Force -Path $packageDir | Out-Null

# Get version from CMakeLists.txt
$VERSION = (Select-String -Path "..\qt_web_editor\CMakeLists.txt" -Pattern 'VERSION ([0-9]+\.[0-9]+\.[0-9]+)').Matches.Groups[1].Value

# Create a temporary directory for the installer files
$TEMP_DIR = "$env:TEMP\RapidInstaller"
New-Item -ItemType Directory -Force -Path $TEMP_DIR | Out-Null

# Copy the application files
$INSTALL_DIR = "$TEMP_DIR\Rapid"
New-Item -ItemType Directory -Force -Path $INSTALL_DIR | Out-Null
Copy-Item -Path "..\build\windows\bin\Release\*" -Destination $INSTALL_DIR -Recurse -Force

# Create a WiX config file
$WXS_CONTENT = @"
<?xml version='1.0' encoding='windows-1252'?>
<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>
    <Product Name='Rapid' Id='*' UpgradeCode='YOUR-UPGRADE-CODE-GOES-HERE' Language='1033' Codepage='1252' Version='$VERSION' Manufacturer='Rapid'>
        <Package Id='*' InstallerVersion='200' Compressed='yes' InstallScope='perMachine' />
        
        <MajorUpgrade DowngradeErrorMessage='A newer version of Rapid is already installed.' />
        
        <Media Id='1' Cabinet='Rapid.cab' EmbedCab='yes' />
        
        <Directory Id='TARGETDIR' Name='SourceDir'>
            <Directory Id='ProgramFilesFolder'>
                <Directory Id='INSTALLFOLDER' Name='Rapid'>
                    <Component Id='MainExecutable' Guid='*'>
                        <File Id='RapidEXE' Name='Rapid.exe' DiskId='1' Source='$INSTALL_DIR\Rapid.exe' KeyPath='yes' />
                        <!-- Add other files as needed -->
                    </Component>
                </Directory>
            </Directory>
            <Directory Id='ProgramMenuFolder'>
                <Directory Id='ProgramMenuDir' Name='Rapid'>
                    <Component Id='ProgramMenuDir' Guid='*'>
                        <Shortcut Id='ApplicationStartMenuShortcut' Name='Rapid' Target='[INSTALLFOLDER]Rapid.exe' WorkingDirectory='INSTALLFOLDER' />
                        <RemoveFolder Id='ProgramMenuDir' On='uninstall' />
                    </Component>
                </Directory>
            </Directory>
        </Directory>
        
        <Feature Id='MainApplication' Title='Rapid' Level='1'>
            <ComponentRef Id='MainExecutable' />
            <ComponentRef Id='ProgramMenuDir' />
        </Feature>
    </Product>
</Wix>
"@

# Save the WiX config file
$WXS_PATH = "$TEMP_DIR\Rapid.wxs"
$WXS_CONTENT | Out-File -FilePath $WXS_PATH -Encoding utf8

# Compile the WiX config
$WIX_OBJ = "$TEMP_DIR\Rapid.wixobj"
& candle.exe -out "$WIX_OBJ" "$WXS_PATH"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile WiX config"
    exit 1
}

# Link the installer
$MSI_PATH = "$PACKAGE_DIR\Rapid-${VERSION}-Windows.msi"
& light.exe -out "$MSI_PATH" "$WIX_OBJ" -ext WixUIExtension
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to create MSI package"
    exit 1
}

# Clean up
Remove-Item -Recurse -Force $TEMP_DIR

Write-Host "Package created: $MSI_PATH"
