#pragma once
#include "Camera/Camera.hpp"
#include "Game/SortGame.hpp"
#include "Graphics/Renderer.hpp"
#include "Gesture/GestureEngine.hpp"
#include "HandTracking/CameraInput.hpp"
#include "HandTracking/MediaPipeHandTracker.hpp"
#include "HandTracking/LandmarkReplay.hpp"
#include "Interaction/InteractionSystem.hpp"
#include "Scene/SceneManager.hpp"
#include "UI/UiLayer.hpp"
#include "Utils/Config.hpp"
#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

struct GLFWwindow;
namespace g3d {
class Application {
 public:
  bool initialize(const AppConfig& config);
  int run();
  void shutdown();
 private:
  static void framebufferCallback(GLFWwindow* window, int width, int height);
  void processKeyboard(float deltaSeconds);
  void trackingLoop();
  void stopTracking();
  GLFWwindow* window_{nullptr};
  AppConfig config_;
  Camera camera_;
  Renderer renderer_;
  SceneManager scene_;
  SortGame game_;
  GestureEngine gestures_;
  InteractionSystem interaction_;
  CameraInput cameraInput_;
  MediaPipeHandTracker tracker_;
  LandmarkReplay replay_;
  UiLayer ui_;
  std::atomic<bool> trackingRunning_{false};
  std::thread trackingThread_;
  std::mutex trackingMutex_;
  HandFrame latestHandFrame_;
  std::uint64_t trackingSequence_{0};
  int width_{1280};
  int height_{720};
};
}
