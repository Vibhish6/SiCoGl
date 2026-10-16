#include "Camera/Camera.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

namespace g3d {
glm::vec3 Camera::forward() const {
  const float y = glm::radians(yaw), p = glm::radians(pitch);
  return glm::normalize(glm::vec3(std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)));
}
glm::mat4 Camera::view() const { return glm::lookAt(position, position + forward(), glm::vec3(0, 1, 0)); }
glm::mat4 Camera::projection(float aspect) const { return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane); }
Ray Camera::rayFromScreen(float x, float y, int width, int height) const {
  const float nx = 2.0f * x / static_cast<float>(width) - 1.0f;
  const float ny = 1.0f - 2.0f * y / static_cast<float>(height);
  const glm::mat4 inverse = glm::inverse(projection(static_cast<float>(width) / static_cast<float>(height)) * view());
  const glm::vec4 nearPoint = inverse * glm::vec4(nx, ny, -1.0f, 1.0f);
  const glm::vec4 farPoint = inverse * glm::vec4(nx, ny, 1.0f, 1.0f);
  const glm::vec3 a = glm::vec3(nearPoint) / nearPoint.w;
  const glm::vec3 b = glm::vec3(farPoint) / farPoint.w;
  return {a, glm::normalize(b - a)};
}
}
