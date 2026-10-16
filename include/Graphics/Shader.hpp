#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace g3d {
class Shader {
 public:
  bool load(const std::string& vertexPath, const std::string& fragmentPath);
  void use() const;
  unsigned int id() const { return program_; }
  void set(const std::string& name, int value) const;
  void set(const std::string& name, float value) const;
  void set(const std::string& name, const glm::vec3& value) const;
  void set(const std::string& name, const glm::mat4& value) const;
 private:
  int location(const std::string& name) const;
  unsigned int program_{0};
  mutable std::unordered_map<std::string, int> locations_;
};
}
