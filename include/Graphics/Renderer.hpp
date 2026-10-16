#pragma once
#include "Camera/Camera.hpp"
#include "Graphics/Shader.hpp"
#include "Scene/SceneManager.hpp"
#include <string>

namespace g3d {
class Renderer {
 public:
  bool initialize(const std::string& assetDir);
  void shutdown();
  void resize(int width, int height);
  void render(const SceneManager& scene, const Camera& camera, const glm::vec3& cursor, bool showGrid);
  int drawCalls() const { return drawCalls_; }
 private:
  void drawGrid(const Camera& camera);
  Shader shader_;
  Shader gridShader_;
  int width_{1280};
  int height_{720};
  int drawCalls_{0};
  unsigned int gridVao_{0};
  unsigned int gridVbo_{0};
  unsigned int cursorVao_{0};
  unsigned int cursorVbo_{0};
  int gridVertexCount_{0};
};
}
