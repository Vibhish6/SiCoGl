#include "Graphics/Shader.hpp"
#include "Utils/Logger.hpp"
#include <fstream>
#include <sstream>
#if GESTURE3D_HAS_GLAD
#include <glad/glad.h>
#endif

namespace g3d {
bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
#if !GESTURE3D_HAS_GLAD
  (void)vertexPath; (void)fragmentPath; log(LogLevel::Error, "OpenGL loader unavailable (glad)"); return false;
#else
  auto read = [](const std::string& path) { std::ifstream f(path); std::stringstream s; s << f.rdbuf(); return s.str(); };
  const std::string vs=read(vertexPath), fs=read(fragmentPath); if (vs.empty() || fs.empty()) { log(LogLevel::Error,"Missing shader asset"); return false; }
  auto compile = [](GLenum type, const std::string& source) { GLuint s=glCreateShader(type); const char* p=source.c_str(); glShaderSource(s,1,&p,nullptr); glCompileShader(s); GLint ok=0; glGetShaderiv(s,GL_COMPILE_STATUS,&ok); if(!ok){char info[1024]{};glGetShaderInfoLog(s,1024,nullptr,info);log(LogLevel::Error,info);glDeleteShader(s);return 0u;} return s; };
  const GLuint v=compile(GL_VERTEX_SHADER,vs), f=compile(GL_FRAGMENT_SHADER,fs); if(!v||!f)return false;
  program_=glCreateProgram(); glAttachShader(program_,v); glAttachShader(program_,f); glLinkProgram(program_); GLint ok=0; glGetProgramiv(program_,GL_LINK_STATUS,&ok); glDeleteShader(v);glDeleteShader(f); if(!ok){char info[1024]{};glGetProgramInfoLog(program_,1024,nullptr,info);log(LogLevel::Error,info);return false;} return true;
#endif
}
void Shader::use() const {
#if GESTURE3D_HAS_GLAD
  glUseProgram(program_);
#endif
}
int Shader::location(const std::string& name) const {
#if GESTURE3D_HAS_GLAD
  if(!locations_.count(name)) locations_[name]=glGetUniformLocation(program_,name.c_str()); return locations_[name];
#else
  (void)name; return -1;
#endif
}
void Shader::set(const std::string& n, int v) const {
#if GESTURE3D_HAS_GLAD
  glUniform1i(location(n), v);
#else
  (void)n; (void)v;
#endif
}
void Shader::set(const std::string& n, float v) const {
#if GESTURE3D_HAS_GLAD
  glUniform1f(location(n), v);
#else
  (void)n; (void)v;
#endif
}
void Shader::set(const std::string& n, const glm::vec3& v) const {
#if GESTURE3D_HAS_GLAD
  glUniform3fv(location(n), 1, &v[0]);
#else
  (void)n; (void)v;
#endif
}
void Shader::set(const std::string& n, const glm::mat4& v) const {
#if GESTURE3D_HAS_GLAD
  glUniformMatrix4fv(location(n), 1, GL_FALSE, &v[0][0]);
#else
  (void)n; (void)v;
#endif
}
}
