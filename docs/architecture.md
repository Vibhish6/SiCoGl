# Architecture

`Application` owns the GLFW lifecycle and frame loop. `CameraInput` owns OpenCV capture and exposes frames without changing scene state. `MediaPipeHandTracker` converts frames to the small `HandFrame` abstraction. `GestureEngine` applies normalized landmark metrics, smoothing, hysteresis thresholds, and hold/cooldown state. `InteractionSystem` converts the normalized cursor through camera unprojection and ray tests, then changes `SceneManager` state. `Renderer` consumes scene state and performs all OpenGL work.

`UiLayer` is an optional Dear ImGui presentation layer. It exposes the gesture/tracking/FPS HUD, scene hierarchy, transform/material editing, primitive creation, duplicate, delete, and history commands without allowing UI code to issue OpenGL draw calls directly.

Scene objects own an ID, primitive type, transform, material, visibility/lock flags, and reusable mesh data. JSON serialization is human-readable and does not serialize GPU handles; meshes are regenerated and uploaded lazily by the renderer.

The MediaPipe dependency is kept at the boundary because MediaPipe C++ is distributed as a Bazel graph rather than a stable single CMake package. With the option enabled, the adapter creates a Tasks HandLandmarker in video mode, converts each OpenCV frame into a CPU `mediapipe::Image`, calls `DetectForVideo`, and copies normalized coordinates and handedness into `HandFrame`. This keeps the core deterministic while making the external ABI explicit.
