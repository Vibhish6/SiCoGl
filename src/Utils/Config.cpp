#include "Utils/Config.hpp"
#include "Utils/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

namespace g3d {
AppConfig loadConfig(const std::string& path) {
  AppConfig config;
  std::ifstream file(path);
  if (!file) { log(LogLevel::Warning, "Configuration not found; using defaults: " + path); return config; }
  try {
    nlohmann::json j; file >> j;
    const auto camera = j.value("camera", nlohmann::json::object());
    config.camera.index = camera.value("index", config.camera.index);
    config.camera.width = camera.value("width", config.camera.width);
    config.camera.height = camera.value("height", config.camera.height);
    config.camera.fps = camera.value("fps", config.camera.fps);
    const auto gesture = j.value("gesture", nlohmann::json::object());
    config.gesture.pinchThreshold = gesture.value("pinchThreshold", config.gesture.pinchThreshold);
    config.gesture.pinchReleaseThreshold = gesture.value("pinchReleaseThreshold", config.gesture.pinchReleaseThreshold);
    config.gesture.smoothingAlpha = gesture.value("smoothingAlpha", config.gesture.smoothingAlpha);
    config.gesture.minimumConfidence = gesture.value("minimumConfidence", config.gesture.minimumConfidence);
    config.gesture.holdFrames = gesture.value("holdFrames", config.gesture.holdFrames);
    config.gesture.deleteCooldownFrames = gesture.value("deleteCooldownFrames", config.gesture.deleteCooldownFrames);
    config.showGrid = j.value("showGrid", config.showGrid);
    config.showDebug = j.value("showDebug", config.showDebug);
    config.demoScene = j.value("demoScene", config.demoScene);
    config.replayFile = j.value("replayFile", config.replayFile);
    config.handLandmarkerModel = j.value("handLandmarkerModel", config.handLandmarkerModel);
  } catch (const std::exception& e) { log(LogLevel::Error, std::string("Invalid configuration: ") + e.what()); }
  return config;
}
}
