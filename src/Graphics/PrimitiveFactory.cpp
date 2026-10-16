#include "Graphics/PrimitiveFactory.hpp"
#include <cmath>

namespace g3d {
namespace {
constexpr float pi = 3.14159265358979323846f;
void quad(MeshData& m, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n) {
  const auto start = static_cast<unsigned int>(m.vertices.size());
  m.vertices.insert(m.vertices.end(), {{a,n,{0,0}},{b,n,{1,0}},{c,n,{1,1}},{d,n,{0,1}}});
  m.indices.insert(m.indices.end(), {start,start+1,start+2,start,start+2,start+3});
}
}
MeshData makePrimitive(PrimitiveType type, int segments, int rings) {
  MeshData m;
  if (type == PrimitiveType::Cube) {
    quad(m,{-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f},{0,0,1});
    quad(m,{.5f,-.5f,-.5f},{-.5f,-.5f,-.5f},{-.5f,.5f,-.5f},{.5f,.5f,-.5f},{0,0,-1});
    quad(m,{-.5f,.5f,.5f},{.5f,.5f,.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f},{0,1,0});
    quad(m,{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,-.5f,.5f},{-.5f,-.5f,.5f},{0,-1,0});
    quad(m,{.5f,-.5f,.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{.5f,.5f,.5f},{1,0,0});
    quad(m,{-.5f,-.5f,-.5f},{-.5f,-.5f,.5f},{-.5f,.5f,.5f},{-.5f,.5f,-.5f},{-1,0,0});
    return m;
  }
  if (type == PrimitiveType::Plane) { quad(m,{-5,0,-5},{5,0,-5},{5,0,5},{-5,0,5},{0,1,0}); return m; }
  if (type == PrimitiveType::Sphere || type == PrimitiveType::Torus) {
    const int major = type == PrimitiveType::Torus ? segments : segments;
    const int minor = type == PrimitiveType::Torus ? rings : rings;
    for (int y=0; y<=minor; ++y) for (int x=0; x<=major; ++x) {
      const float u = 2*pi*x/major, v = 2*pi*y/minor;
      float r = type == PrimitiveType::Torus ? 0.30f : 0.5f;
      glm::vec3 p;
      if (type == PrimitiveType::Torus) p = {(0.5f+r*std::cos(v))*std::cos(u), r*std::sin(v), (0.5f+r*std::cos(v))*std::sin(u)};
      else p = {0.5f*std::sin(v)*std::cos(u), 0.5f*std::cos(v), 0.5f*std::sin(v)*std::sin(u)};
      glm::vec3 n = type == PrimitiveType::Torus ? glm::normalize(p - glm::vec3(0.5f*std::cos(u),0,0.5f*std::sin(u))) : glm::normalize(p);
      m.vertices.push_back({p,n,{static_cast<float>(x)/major,static_cast<float>(y)/minor}});
    }
    for (int y=0; y<minor; ++y) for (int x=0; x<major; ++x) { const unsigned int i=y*(major+1)+x; m.indices.insert(m.indices.end(),{i,i+1,i+major+1,i+1,i+major+2,i+major+1}); }
    return m;
  }
  const float top = type == PrimitiveType::Cone ? 0.0f : 0.5f;
  for (int y=0; y<=rings; ++y) {
    const float t=static_cast<float>(y)/rings, radius=(1-t)*0.5f, h=t-0.5f;
    for (int x=0; x<=segments; ++x) { const float a=2*pi*x/segments; glm::vec3 p{radius*std::cos(a),h,radius*std::sin(a)}; glm::vec3 n=glm::normalize(glm::vec3(std::cos(a), type==PrimitiveType::Cone ? 0.5f : 0.0f, std::sin(a))); m.vertices.push_back({p,n,{static_cast<float>(x)/segments,t}}); }
  }
  for (int y=0; y<rings; ++y) for (int x=0; x<segments; ++x) { const unsigned int i=y*(segments+1)+x; m.indices.insert(m.indices.end(),{i,i+1,i+segments+1,i+1,i+segments+2,i+segments+1}); }
  return m;
}
}
