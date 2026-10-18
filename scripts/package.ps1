param(
    [string]$Version = "1.0.0",
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " Building and Packaging AI-Pwd v$Version  " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

$rootDir = Split-Path -Parent $PSScriptRoot
Set-Location $rootDir

# 1. Compile Release executable
Write-Host "[1/4] Compiling AI-Pwd ($Config)..." -ForegroundColor Yellow
cmake --build build --config $Config
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

# 2. Setup Staging Directory
$distDir = Join-Path $rootDir "dist"
$packageName = "ai-pwd-v$Version-windows-x64"
$stageDir = Join-Path $distDir $packageName
$zipPath = Join-Path $distDir "$packageName.zip"
$shaPath = Join-Path $distDir "$packageName.zip.sha256"

if (Test-Path $stageDir) {
    Remove-Item $stageDir -Recurse -Force
}
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}
New-Item -ItemType Directory -Path $stageDir -Force | Out-Null

# 3. Copy binaries and assets
Write-Host "[2/4] Collecting application binaries and resources..." -ForegroundColor Yellow
Copy-Item (Join-Path $rootDir "build\$Config\ai_pwd.exe") -Destination $stageDir
New-Item -ItemType Directory -Path (Join-Path $stageDir "data") -Force | Out-Null
if (Test-Path (Join-Path $rootDir "README.md")) {
    Copy-Item (Join-Path $rootDir "README.md") -Destination $stageDir
}
if (Test-Path (Join-Path $rootDir "LICENSE")) {
    Copy-Item (Join-Path $rootDir "LICENSE") -Destination $stageDir
}
Copy-Item (Join-Path $rootDir "build\$Config\ai_pwd.exe") -Destination (Join-Path $distDir "ai_pwd.exe")

# 4. Create Portable ZIP Archive
Write-Host "[3/4] Creating ZIP distribution archive..." -ForegroundColor Yellow
Compress-Archive -Path "$stageDir\*" -DestinationPath $zipPath -CompressionLevel Optimal

# 5. Generate Checksum
Write-Host "[4/4] Generating SHA256 checksum..." -ForegroundColor Yellow
$hash = (Get-FileHash -Path $zipPath -Algorithm SHA256).Hash
Set-Content -Path $shaPath -Value "$hash  $packageName.zip"

$zipSize = (Get-Item $zipPath).Length / 1MB
Write-Host "==========================================" -ForegroundColor Green
Write-Host " Packaging completed successfully!        " -ForegroundColor Green
Write-Host " Archive:  $zipPath" -ForegroundColor Green
Write-Host " Size:     $([math]::Round($zipSize, 2)) MB" -ForegroundColor Green
Write-Host " SHA256:   $hash" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Green
