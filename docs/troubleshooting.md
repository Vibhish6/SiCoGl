# Troubleshooting

**No window / OpenGL loader error:** install `glad`, use a current GPU driver, and verify that the machine exposes OpenGL 3.3 or newer.

**Webcam unavailable:** close other camera applications, check Windows camera privacy permissions, then change `camera.index` in `config/config.json`.

**Tracking disabled:** run `\.\scripts\run-live.ps1` to start the Python MediaPipe bridge. The native C++ adapter remains available for builds with a prepared MediaPipe C++ checkout and `GESTURE3D_WITH_MEDIAPIPE=ON`.

**Jitter:** lower `gesture.smoothingAlpha` for lower latency or increase it for smoother movement; adjust pinch thresholds only after checking normalized pinch distance.

**Objects jump when grabbed:** the implementation preserves the cursor-to-object offset. If the cursor is on the wrong depth plane, use the interaction-plane/depth configuration appropriate to the demonstration setup.
