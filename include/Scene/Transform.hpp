#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace g3d {
struct Transform {
  glm::vec3 position{0.0f};
  glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};

  glm::mat4 modelMatrix() const {
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation) * glm::scale(glm::mat4(1.0f), scale);
  }
  glm::vec3 eulerDegrees() const { return glm::degrees(glm::eulerAngles(orientation)); }
  void setEulerDegrees(const glm::vec3& degrees) { orientation = glm::quat(glm::radians(degrees)); }
};
}
