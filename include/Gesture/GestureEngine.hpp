#pragma once
#include "HandTracking/HandTypes.hpp"
#include <glm/glm.hpp>
#include <string>

namespace g3d {
enum class Gesture { None, Point, Pinch, OpenPalm, Fist, TwoHandPinch, TwoFinger, ThreeFinger };
struct GestureState {
  Gesture gesture{Gesture::None};
  glm::vec2 cursor{0.5f};
  float confidence{0.0f};
  bool pinchStarted{false};
  bool pinchReleased{false};
  bool twoHandPinchStarted{false};
  bool twoHandPinchReleased{false};
  bool deleteConfirmed{false};
  bool hasTwoHands{false};
  int handCount{0};
  glm::vec3 handVector{};
  float handDistance{0.0f};
  float handAngle{0.0f};
  float primaryRoll{0.0f};
};
struct GestureConfig {
  float pinchThreshold{0.32f};
  float pinchReleaseThreshold{0.42f};
  float smoothingAlpha{0.35f};
  float minimumConfidence{0.55f};
  int holdFrames{8};
  int deleteCooldownFrames{30};
};

class GestureEngine {
 public:
  explicit GestureEngine(GestureConfig config = {});
  GestureState update(const HandFrame& frame);
  void reset();
  static std::string toString(Gesture gesture);
 private:
  Gesture classify(const Hand& hand) const;
  GestureConfig config_;
  bool pinchActive_{false};
  bool twoHandPinchActive_{false};
  int fistFrames_{0};
  int deleteCooldown_{0};
  glm::vec2 smoothedCursor_{0.5f};
  bool cursorInitialized_{false};
  Gesture previousGesture_{Gesture::None};
};
}
