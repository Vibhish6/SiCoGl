#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUv;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(){
  worldPosition=vec3(model*vec4(aPosition,1.0));
  worldNormal=mat3(transpose(inverse(model)))*aNormal;
  uv=aUv;
  gl_Position=projection*view*vec4(worldPosition,1.0);
}
