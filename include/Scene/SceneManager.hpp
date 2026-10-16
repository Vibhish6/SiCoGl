#pragma once
#include "SceneObject.hpp"
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace g3d {
struct Ray { glm::vec3 origin; glm::vec3 direction; };
struct RayHit { std::uint64_t objectId; float distance; glm::vec3 point; };

class SceneManager {
 public:
  SceneManager();
  SceneObject& create(PrimitiveType type, const glm::vec3& position = {});
  SceneObject* duplicate(std::uint64_t id, const glm::vec3& offset = {0.35f, 0.0f, 0.35f});
  bool remove(std::uint64_t id);
  bool setMaterial(std::uint64_t id, const Material& material);
  SceneObject* find(std::uint64_t id);
  const SceneObject* find(std::uint64_t id) const;
  void select(std::optional<std::uint64_t> id);
  SceneObject* selected();
  const std::vector<std::shared_ptr<SceneObject>>& objects() const { return objects_; }
  std::optional<RayHit> hitTest(const Ray& ray) const;
  bool save(const std::string& path) const;
  bool load(const std::string& path);
  void beginEdit();
  void endEdit();
  bool undo();
  bool redo();
  bool canUndo() const { return historyIndex_ > 0; }
  bool canRedo() const { return historyIndex_ + 1 < history_.size(); }
  void clear();
 private:
  std::string serialize() const;
  bool restore(const std::string& content);
  void recordMutation(const std::string& before);
  std::vector<std::shared_ptr<SceneObject>> objects_;
  std::uint64_t nextId_{1};
  std::vector<std::string> history_;
  std::size_t historyIndex_{0};
  std::string editBefore_;
};
}
