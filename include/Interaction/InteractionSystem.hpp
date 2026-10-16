#pragma once
#include "Camera/Camera.hpp"
#include "Gesture/GestureEngine.hpp"
#include "Scene/SceneManager.hpp"
#include <cstdint>
#include <optional>

namespace g3d {
class SortGame;
enum class InteractionMode { Select, Translate, Rotate, Scale, Create, Delete, Camera };
class InteractionSystem {
 public:
  void setMode(InteractionMode mode) { mode_=mode; }
  void setCreateType(PrimitiveType type) { createType_=type; }
  void setAxisLock(std::optional<int> axis) { axisLock_=axis; }
  InteractionMode mode() const { return mode_; }
  void update(const GestureState& gesture, SceneManager& scene, const Camera& camera, int width, int height, float deltaSeconds, SortGame* game = nullptr);
  const glm::vec3& cursorWorld() const { return cursorWorld_; }
  std::uint64_t grabbedId() const { return grabbedId_; }
 private:
  glm::vec3 cursorWorld_{};
  std::uint64_t grabbedId_{0};
  glm::vec3 grabOffset_{};
  glm::vec3 smoothedGrabPosition_{};
  float grabPlaneZ_{0.0f};
  bool grabPositionInitialized_{false};
  bool manipulating_{false};
  float initialTwoHandDistance_{0.0f};
  glm::vec3 initialScale_{1.0f};
  glm::vec3 initialPosition_{};
  glm::quat initialOrientation_{1.0f, 0.0f, 0.0f, 0.0f};
  float initialHandAngle_{0.0f};
  float initialPrimaryRoll_{0.0f};
  float smoothedTwoHandDistance_{0.0f};
  bool twoHandActive_{false};
  InteractionMode mode_{InteractionMode::Translate};
  PrimitiveType createType_{PrimitiveType::Cube};
  std::optional<int> axisLock_;
};
}
