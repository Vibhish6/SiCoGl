param([ValidateSet('Debug','Release')][string]$Configuration = 'Release')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$python = Join-Path $root '.venv\Scripts\python.exe'
$model = Join-Path $root 'assets\hand_landmarker.task'
$exe = Join-Path $root "build-mingw-vcpkg\bin\gesture3d.exe"
$readyFile = Join-Path $root '.gesture3d-bridge.ready'
$config = Get-Content (Join-Path $root 'config\config.json') -Raw | ConvertFrom-Json
if (-not (Test-Path $python)) { throw 'Python environment missing. Run: python -m venv .venv; .\.venv\Scripts\pip.exe install mediapipe opencv-python' }
if (-not (Test-Path $model)) { throw "Missing model: $model" }
& (Join-Path $PSScriptRoot 'build-live.ps1') -Configuration $Configuration
if (Test-Path $readyFile) { [System.IO.File]::Delete($readyFile) }
$bridge = Start-Process -FilePath $python -ArgumentList @((Join-Path $PSScriptRoot 'mediapipe_bridge.py'), '--model', $model, '--camera', $config.camera.index, '--width', $config.camera.width, '--height', $config.camera.height, '--fps', $config.camera.fps, '--smoothing', $config.gesture.smoothingAlpha, '--preview', '--ready-file', $readyFile) -WorkingDirectory $root -WindowStyle Normal -PassThru
try {
  for ($i = 0; $i -lt 100; $i++) {
    if (Test-Path $readyFile) { break }
    Start-Sleep -Milliseconds 100
  }
  if (-not (Test-Path $readyFile)) { throw 'MediaPipe bridge did not become ready.' }
  $env:Path = "$(Join-Path $root '.tools\mingw64\bin');$env:Path"
  & $exe
} finally {
  if ($bridge -and -not $bridge.HasExited) { Stop-Process -Id $bridge.Id -Force }
  if (Test-Path $readyFile) { [System.IO.File]::Delete($readyFile) }
}
