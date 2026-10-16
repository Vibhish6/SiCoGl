# Gesture3D

Gesture3D is a C++17 OpenGL 3.3 core-profile modeling sandbox. It is organized around a strict pipeline:

`camera input → hand landmarks → gesture state → interaction command → scene state → OpenGL renderer`

The implementation includes procedural cube, sphere, cylinder, cone, plane, and torus meshes; GLM transforms; perspective camera/unprojection; ray/AABB selection; Phong lighting; grid rendering; JSON scene files; temporal gesture logic; scene history; replay input; and deterministic core tests.

## Status

OpenCV webcam capture is used when OpenCV is found. The MediaPipe C++ Tasks adapter is isolated behind `GESTURE3D_WITH_MEDIAPIPE`; live tracking additionally requires the MediaPipe C++ SDK, its linked Bazel-built libraries, and `assets/hand_landmarker.task`. Without those external runtime dependencies, the replay mode remains available but live tracking is not claimed.

## Requirements

- Windows 10/11
- Visual Studio 2022 with Desktop C++ workload
- CMake 3.21+
- OpenGL 3.3-capable GPU and webcam
- `glad` installed through vcpkg or another CMake package
- Dear ImGui (downloaded automatically by CMake)
- OpenCV 4.x (optional for the renderer-only build; required for webcam capture)
- MediaPipe C++ SDK (required for live tracking)
- Python 3.12+ with `mediapipe` and `opencv-python` (supported Windows fallback)

GLFW, GLM, and nlohmann/json are downloaded by CMake FetchContent on the first configure.

## Build and run

```powershell
cmake -S . -B build
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\bin\Release\gesture3d.exe
# deterministic landmark replay without a webcam/MediaPipe runtime
.\build\bin\Release\gesture3d.exe --replay tests/data/point_replay.json
# live webcam tracking on Windows without the native MediaPipe C++ SDK
.\scripts\run-live.ps1
```

The live command starts the local Python MediaPipe bridge, opens the default webcam, and feeds hand landmarks into the C++ application. The one-time setup is:

```powershell
python -m venv .venv
.\.venv\Scripts\pip.exe install mediapipe opencv-python
```

The convenient equivalent is `.\scripts\build.ps1`, followed by `.\scripts\run.ps1`.

## Controls

- `W/A/S/D`: move the debug camera
- `Delete`: delete the selected object
- `Ctrl+S`: save `scenes/user_scene.json`
- `Ctrl+Z` / `Ctrl+Y`: undo/redo
- `C`: duplicate selected object; `M`: cycle material appearance
- `1`–`6`: create cube/sphere/cylinder/cone/torus/plane
- `N`: start a new Sort & Drop physics run
- `T` / `R` / `G`: translate/rotate/scale mode; `P` then pinch: gesture-create a cube
- Point: cursor/selection
- Pinch: grab and translate on the configured interaction plane
- Two-hand pinch: scale the grabbed object; twist the hand pair to rotate it
- Fist held for several frames: deliberate deletion confirmation

The live UI includes a Sort Lab panel with tracking confidence, detected hand count, gesture state, object list, score, and run progress. Pinch falling items, carry them over the open container, and release them; gravity and bounce physics finish the drop.

See [docs/gestures.md](docs/gestures.md), [docs/build.md](docs/build.md), and [docs/troubleshooting.md](docs/troubleshooting.md).
