#version 330 core
struct Material { vec3 albedo; float metallic; float roughness; float alpha; vec3 emissive; };
struct Light { vec3 position; vec3 color; };
in vec3 worldPosition;
in vec3 worldNormal;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;
out vec4 fragColor;
void main(){
  vec3 n=normalize(worldNormal); vec3 l=normalize(light.position-worldPosition); vec3 v=normalize(viewPos-worldPosition); vec3 h=normalize(l+v);
  float diffuse=max(dot(n,l),0.0); float shininess=mix(8.0,128.0,1.0-material.roughness); float specular=pow(max(dot(n,h),0.0),shininess);
  vec3 ambient=0.08*material.albedo; vec3 color=ambient+diffuse*material.albedo*light.color+specular*mix(vec3(0.35),material.albedo,material.metallic)*light.color+material.emissive;
  fragColor=vec4(color,material.alpha);
}
