param([ValidateSet('Debug','Release')][string]$Configuration = 'Release')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$cmake = Join-Path $root '.tools\cmake-4.4.3-windows-x86_64\bin\cmake.exe'
$mingw = Join-Path $root '.tools\mingw64\bin'
$toolchain = Join-Path $root '.tools\vcpkg\scripts\buildsystems\vcpkg.cmake'
$env:Path = "$mingw;$(Split-Path $cmake);$env:Path"
& $cmake -S $root -B (Join-Path $root 'build-mingw-vcpkg') -G 'MinGW Makefiles' -DCMAKE_BUILD_TYPE=$Configuration -DCMAKE_TOOLCHAIN_FILE=$toolchain -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DGESTURE3D_WITH_OPENCV=OFF -DGESTURE3D_WITH_MEDIAPIPE=OFF -DGESTURE3D_WITH_PYTHON_MEDIAPIPE=ON -DGESTURE3D_WITH_IMGUI=ON
& $cmake --build (Join-Path $root 'build-mingw-vcpkg') --parallel
