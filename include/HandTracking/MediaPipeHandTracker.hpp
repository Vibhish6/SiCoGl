#pragma once
#include "HandTracking/HandTypes.hpp"
#include <string>
#include <cstdint>
#include <memory>
#include <atomic>

#if GESTURE3D_HAS_MEDIAPIPE
#include "mediapipe/tasks/cc/vision/hand_landmarker/hand_landmarker.h"
#endif

namespace g3d {
class MediaPipeHandTracker {
 public:
  ~MediaPipeHandTracker();
  bool initialize(const std::string& modelPath, int maxHands = 2);
  bool process(const void* frame, HandFrame& output);
  bool available() const { return available_.load(std::memory_order_relaxed); }
  bool usesExternalCamera() const { return bridgeConnected_.load(std::memory_order_relaxed); }
  std::string status() const;
 private:
  std::atomic<bool> available_{false};
  std::atomic<bool> bridgeConnected_{false};
  std::intptr_t bridgeSocket_{-1};
  std::string bridgeReceiveBuffer_;
  std::int64_t timestampMs_{0};
#if GESTURE3D_HAS_MEDIAPIPE
  std::unique_ptr<mediapipe::tasks::vision::hand_landmarker::HandLandmarker> landmarker_;
#endif
};
}
