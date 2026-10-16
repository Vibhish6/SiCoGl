#pragma once
#include "Gesture/GestureEngine.hpp"
#include "Game/SortGame.hpp"
#include "Scene/SceneManager.hpp"
#include <string>

struct GLFWwindow;
namespace g3d {
class UiLayer {
 public:
  bool initialize(GLFWwindow* window);
  void render(SceneManager& scene, SortGame& game, const GestureState& gesture, const std::string& trackingStatus, float fps);
  void shutdown();
 private:
  bool initialized_{false};
};
}
