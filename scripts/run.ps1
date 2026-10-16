param([string]$Configuration = 'Release', [string]$ConfigPath = '')
$ErrorActionPreference = 'Stop'
$exe = Join-Path $PSScriptRoot "../build/bin/$Configuration/gesture3d.exe"
if (-not (Test-Path $exe)) { & (Join-Path $PSScriptRoot 'build.ps1') -Configuration $Configuration }
if ($ConfigPath) { & $exe $ConfigPath } else { & $exe }
