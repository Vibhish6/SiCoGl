#include "Graphics/Renderer.hpp"
#include "Utils/Logger.hpp"
#include <filesystem>
#include <vector>
#include <algorithm>
#if GESTURE3D_HAS_GLAD
#include <glad/glad.h>
#endif

namespace g3d {
bool Renderer::initialize(const std::string& assetDir) {
#if !GESTURE3D_HAS_GLAD
  (void)assetDir; log(LogLevel::Error,"Renderer requires glad/OpenGL function loading"); return false;
#else
  const std::filesystem::path root(assetDir);
  if(!shader_.load((root/"shaders/basic.vert").string(),(root/"shaders/phong.frag").string()))return false;
  if(!gridShader_.load((root/"shaders/grid.vert").string(),(root/"shaders/grid.frag").string()))return false;
  glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);glCullFace(GL_BACK);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glClearColor(.025f,.035f,.06f,1);

  std::vector<glm::vec3> gridVertices;
  gridVertices.reserve(84);
  for(int i=-10;i<=10;++i){gridVertices.push_back({static_cast<float>(i),0,-10});gridVertices.push_back({static_cast<float>(i),0,10});gridVertices.push_back({-10,0,static_cast<float>(i)});gridVertices.push_back({10,0,static_cast<float>(i)});}
  gridVertexCount_=static_cast<int>(gridVertices.size());
  glGenVertexArrays(1,&gridVao_);glGenBuffers(1,&gridVbo_);glBindVertexArray(gridVao_);glBindBuffer(GL_ARRAY_BUFFER,gridVbo_);glBufferData(GL_ARRAY_BUFFER,gridVertices.size()*sizeof(glm::vec3),gridVertices.data(),GL_STATIC_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),nullptr);glEnableVertexAttribArray(0);

  const float initialCursor[]={0.0f,0.0f,0.0f};
  glGenVertexArrays(1,&cursorVao_);glGenBuffers(1,&cursorVbo_);glBindVertexArray(cursorVao_);glBindBuffer(GL_ARRAY_BUFFER,cursorVbo_);glBufferData(GL_ARRAY_BUFFER,sizeof(initialCursor),initialCursor,GL_STREAM_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,nullptr);glEnableVertexAttribArray(0);
  glBindVertexArray(0);glBindBuffer(GL_ARRAY_BUFFER,0);
  return true;
#endif
}
void Renderer::shutdown() {
#if GESTURE3D_HAS_GLAD
  if(gridVbo_!=0)glDeleteBuffers(1,&gridVbo_);if(gridVao_!=0)glDeleteVertexArrays(1,&gridVao_);if(cursorVbo_!=0)glDeleteBuffers(1,&cursorVbo_);if(cursorVao_!=0)glDeleteVertexArrays(1,&cursorVao_);
  gridVbo_=0;gridVao_=0;cursorVbo_=0;cursorVao_=0;gridVertexCount_=0;
#endif
}
void Renderer::resize(int width,int height){width_=std::max(1,width);height_=std::max(1,height);
#if GESTURE3D_HAS_GLAD
  glViewport(0,0,width,height);
#endif
}
void Renderer::render(const SceneManager& scene,const Camera& camera,const glm::vec3& cursor,bool showGrid){
#if GESTURE3D_HAS_GLAD
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); drawCalls_=0; const glm::mat4 view=camera.view(),projection=camera.projection(static_cast<float>(width_)/height_); shader_.use();shader_.set("view",view);shader_.set("projection",projection);shader_.set("light.position",glm::vec3(3,6,4));shader_.set("light.color",glm::vec3(1));shader_.set("viewPos",camera.position);
  for(const auto&o:scene.objects()){if(!o->visible||!o->mesh)continue;auto&m=*o->mesh;if(!m.uploaded){glGenVertexArrays(1,&m.vao);glGenBuffers(1,&m.vbo);glGenBuffers(1,&m.ebo);glBindVertexArray(m.vao);glBindBuffer(GL_ARRAY_BUFFER,m.vbo);glBufferData(GL_ARRAY_BUFFER,m.data.vertices.size()*sizeof(Vertex),m.data.vertices.data(),GL_STATIC_DRAW);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.ebo);glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.data.indices.size()*sizeof(unsigned int),m.data.indices.data(),GL_STATIC_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);glEnableVertexAttribArray(0);glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(sizeof(glm::vec3)));glEnableVertexAttribArray(1);glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(2*sizeof(glm::vec3)));glEnableVertexAttribArray(2);m.uploaded=true;}shader_.set("model",o->transform.modelMatrix());shader_.set("material.albedo",o->selected?glm::vec3(0.25f,0.9f,1.0f):o->material.baseColor);shader_.set("material.metallic",o->material.metallic);shader_.set("material.roughness",o->material.roughness);shader_.set("material.alpha",o->material.alpha);shader_.set("material.emissive",o->material.emissive);glBindVertexArray(m.vao);glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(m.data.indices.size()),GL_UNSIGNED_INT,nullptr);++drawCalls_;if(o->selected){glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);shader_.set("material.albedo",glm::vec3(0.3f,0.9f,1.0f));glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(m.data.indices.size()),GL_UNSIGNED_INT,nullptr);glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);++drawCalls_;}}
  if(showGrid)drawGrid(camera);
  glPointSize(12.0f);const float point[]={cursor.x,cursor.y,cursor.z};glBindVertexArray(cursorVao_);glBindBuffer(GL_ARRAY_BUFFER,cursorVbo_);glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(point),point);gridShader_.use();gridShader_.set("view",view);gridShader_.set("projection",projection);gridShader_.set("color",glm::vec3(0.2f,0.9f,1.0f));glDrawArrays(GL_POINTS,0,1);glBindVertexArray(0);
#else
  (void)scene;(void)camera;(void)cursor;(void)showGrid;
#endif
}
void Renderer::drawGrid(const Camera& camera){
#if GESTURE3D_HAS_GLAD
  gridShader_.use();gridShader_.set("view",camera.view());gridShader_.set("projection",camera.projection(static_cast<float>(width_)/height_));gridShader_.set("color",glm::vec3(.14f,.19f,.28f));glBindVertexArray(gridVao_);glDrawArrays(GL_LINES,0,gridVertexCount_);glBindVertexArray(0);
#else
  (void)camera;
#endif
}
}
