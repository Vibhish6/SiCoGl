#pragma once
#include "Scene/SceneManager.hpp"
#include <glm/glm.hpp>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace g3d {
struct GameInfo {
  bool active{false};
  int placed{0};
  int total{0};
  int score{0};
};

class SortGame {
 public:
  void start(SceneManager& scene);
  void update(float deltaSeconds, SceneManager& scene);
  void onGrab(std::uint64_t id);
  void onRelease(std::uint64_t id);
  bool canGrab(const SceneObject& object) const;
  bool active() const { return active_; }
  GameInfo info() const;

 private:
  bool active_{false};
  int score_{0};
  std::unordered_set<std::uint64_t> itemIds_;
  std::unordered_set<std::uint64_t> placedIds_;
  std::unordered_set<std::uint64_t> heldIds_;
  std::unordered_map<std::uint64_t, glm::vec3> velocity_;
};
}
