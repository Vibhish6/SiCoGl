# Windows build

Install Visual Studio 2022 Desktop C++, CMake, Git, and vcpkg. Install the OpenGL loader and OpenCV packages, for example:

```powershell
vcpkg install glad:x64-windows opencv4:x64-windows
```

Configure with the vcpkg toolchain:

```powershell
cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

For a renderer-only build, CMake can continue without OpenCV, but live webcam tracking is unavailable. To enable the MediaPipe adapter, configure with `-DGESTURE3D_WITH_MEDIAPIPE=ON -DMEDIAPIPE_ROOT=C:/path/to/mediapipe -DMEDIAPIPE_LIBRARIES=...` after preparing the MediaPipe C++ Tasks SDK and its Bazel-built libraries. Place a compatible `hand_landmarker.task` model in `assets/`; the C++ Tasks API requires a model asset and supports video timestamps for frame-by-frame detection.

On Windows without the native MediaPipe C++ SDK, the repository also supports a Python bridge. Install Python 3.12+, then run `python -m venv .venv` and `\.venv\Scripts\pip.exe install mediapipe opencv-python`. With the model in `assets/hand_landmarker.task`, run `\.\scripts\run-live.ps1`.
