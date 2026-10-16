#pragma once
#include <glm/glm.hpp>
#include <array>
#include <cstdint>
#include <vector>

namespace g3d {
struct Hand {
  std::array<glm::vec3, 21> landmarks{};
  bool left{false};
  float confidence{0.0f};
  bool valid{false};
  glm::vec3 palmCenter() const { return (landmarks[0] + landmarks[5] + landmarks[9] + landmarks[13] + landmarks[17]) / 5.0f; }
  float pinchDistance() const { return glm::distance(landmarks[4], landmarks[8]); }
  float sizeEstimate() const { return glm::distance(landmarks[0], landmarks[9]); }
  glm::vec3 palmNormal() const { return glm::normalize(glm::cross(landmarks[5]-landmarks[0],landmarks[17]-landmarks[0])); }
  std::array<bool,5> fingerExtended() const { return {landmarks[8].y<landmarks[6].y,landmarks[12].y<landmarks[10].y,landmarks[16].y<landmarks[14].y,landmarks[20].y<landmarks[18].y,landmarks[4].x>landmarks[3].x}; }
};
struct HandFrame { std::vector<Hand> hands; std::uint64_t timestampMs{0}; };
}
