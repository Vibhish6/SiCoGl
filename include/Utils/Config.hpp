#pragma once
#include "Gesture/GestureEngine.hpp"
#include "HandTracking/CameraInput.hpp"
#include <string>

namespace g3d {
struct AppConfig {
  CameraConfig camera;
  GestureConfig gesture;
  bool showGrid{true};
  bool showDebug{false};
  std::string demoScene{"scenes/demo_scene.json"};
  std::string replayFile;
  std::string handLandmarkerModel{"assets/hand_landmarker.task"};
};
AppConfig loadConfig(const std::string& path);
}
