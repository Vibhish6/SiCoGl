#include "Scene/SceneManager.hpp"
#include "Graphics/PrimitiveFactory.hpp"
#include "Utils/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <cstdint>

namespace g3d {
namespace {
std::string typeName(PrimitiveType t) { switch(t){case PrimitiveType::Cube:return"cube";case PrimitiveType::Sphere:return"sphere";case PrimitiveType::Cylinder:return"cylinder";case PrimitiveType::Cone:return"cone";case PrimitiveType::Plane:return"plane";case PrimitiveType::Torus:return"torus";} return"cube"; }
PrimitiveType parseType(const std::string& s) { if(s=="sphere")return PrimitiveType::Sphere;if(s=="cylinder")return PrimitiveType::Cylinder;if(s=="cone")return PrimitiveType::Cone;if(s=="plane")return PrimitiveType::Plane;if(s=="torus")return PrimitiveType::Torus;return PrimitiveType::Cube; }
std::string materialName(MaterialType t) { switch(t){case MaterialType::Matte:return"matte";case MaterialType::Glossy:return"glossy";case MaterialType::Metallic:return"metallic";case MaterialType::Glass:return"glass";case MaterialType::Emissive:return"emissive";}return"matte"; }
MaterialType parseMaterial(const std::string& s) { if(s=="glossy")return MaterialType::Glossy;if(s=="metallic")return MaterialType::Metallic;if(s=="glass")return MaterialType::Glass;if(s=="emissive")return MaterialType::Emissive;return MaterialType::Matte; }
nlohmann::json vec3(const glm::vec3& v) { return {v.x,v.y,v.z}; }
glm::vec3 vec3(const nlohmann::json& j, glm::vec3 fallback) { if(!j.is_array()||j.size()!=3)return fallback;return {j[0].get<float>(),j[1].get<float>(),j[2].get<float>()}; }
}
SceneManager::SceneManager() { history_.push_back(serialize()); }
SceneObject& SceneManager::create(PrimitiveType type, const glm::vec3& position) {
  const std::string before=serialize();
  auto object=std::make_shared<SceneObject>(); object->id=nextId_++; object->primitive=type;
  object->name=typeName(type)+"_"+std::to_string(object->id); object->mesh=std::make_shared<Mesh>(); object->mesh->data=makePrimitive(type); object->transform.position=position;
  objects_.push_back(object); select(object->id); recordMutation(before); log(LogLevel::Info,"Created "+object->name); return *object;
}
SceneObject* SceneManager::duplicate(std::uint64_t id, const glm::vec3& offset) { auto*source=find(id);if(!source)return nullptr;const std::string before=serialize();auto copy=std::make_shared<SceneObject>(*source);copy->id=nextId_++;copy->name=source->name+"_copy";copy->transform.position+=offset;copy->selected=false;copy->mesh=std::make_shared<Mesh>(*source->mesh);copy->mesh->uploaded=false;objects_.push_back(copy);select(copy->id);recordMutation(before);return copy.get(); }
bool SceneManager::remove(std::uint64_t id) { const std::string before=serialize(); for(auto it=objects_.begin();it!=objects_.end();++it) if((*it)->id==id){log(LogLevel::Info,"Deleted "+(*it)->name);objects_.erase(it);recordMutation(before);return true;}return false; }
bool SceneManager::setMaterial(std::uint64_t id,const Material& material){auto*o=find(id);if(!o)return false;const std::string before=serialize();o->material=material;recordMutation(before);return true;}
SceneObject* SceneManager::find(std::uint64_t id) { for(auto& o:objects_)if(o->id==id)return o.get();return nullptr; }
const SceneObject* SceneManager::find(std::uint64_t id) const { for(const auto& o:objects_)if(o->id==id)return o.get();return nullptr; }
void SceneManager::select(std::optional<std::uint64_t> id) { for(auto& o:objects_)o->selected=id&&o->id==*id; }
SceneObject* SceneManager::selected() { for(auto& o:objects_)if(o->selected)return o.get();return nullptr; }
std::optional<RayHit> SceneManager::hitTest(const Ray& ray) const {
  if (!std::isfinite(ray.origin.x) || !std::isfinite(ray.origin.y) || !std::isfinite(ray.origin.z) || !std::isfinite(ray.direction.x) || !std::isfinite(ray.direction.y) || !std::isfinite(ray.direction.z)) return std::nullopt;
  std::optional<RayHit> best;
  for (const auto& o : objects_) {
    if (!o->visible || o->locked) continue;
    if (glm::any(glm::lessThan(glm::abs(o->transform.scale), glm::vec3(1e-6f)))) continue;

    // Intersect the unscaled local AABB. Transforming the ray preserves the
    // world-ray parameter, so the resulting distance remains comparable across
    // objects even when their scales differ.
    const glm::mat4 inverseModel = glm::inverse(o->transform.modelMatrix());
    const glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(ray.origin, 1.0f));
    const glm::vec3 localDirection = glm::vec3(inverseModel * glm::vec4(ray.direction, 0.0f));
    const glm::vec3 min = -o->localHalfExtents;
    const glm::vec3 max = o->localHalfExtents;
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    for (int i = 0; i < 3; ++i) {
      if (std::abs(localDirection[i]) < 1e-6f) {
        if (localOrigin[i] < min[i] || localOrigin[i] > max[i]) {
          tmin = 1.0f;
          tmax = 0.0f;
          break;
        }
      } else {
        float a = (min[i] - localOrigin[i]) / localDirection[i];
        float b = (max[i] - localOrigin[i]) / localDirection[i];
        if (a > b) std::swap(a, b);
        tmin = std::max(tmin, a);
        tmax = std::min(tmax, b);
      }
    }
    if (tmax >= tmin) {
      const float distance = tmin;
      if (!best || distance < best->distance) {
        best = RayHit{o->id, distance, ray.origin + ray.direction * distance};
      }
    }
  }
  return best;
}
std::string SceneManager::serialize() const { nlohmann::json root;root["version"]=1;root["objects"]=nlohmann::json::array();for(const auto&o:objects_){root["objects"].push_back({{"id",o->id},{"name",o->name},{"type",typeName(o->primitive)},{"position",vec3(o->transform.position)},{"rotation",vec3(o->transform.eulerDegrees())},{"scale",vec3(o->transform.scale)},{"visible",o->visible},{"locked",o->locked},{"material",materialName(o->material.type)},{"color",vec3(o->material.baseColor)},{"metallic",o->material.metallic},{"roughness",o->material.roughness},{"alpha",o->material.alpha},{"emissive",vec3(o->material.emissive)}});}return root.dump(); }
bool SceneManager::save(const std::string& path) const { std::ofstream f(path);if(!f)return false;f<<nlohmann::json::parse(serialize()).dump(2);return true; }
bool SceneManager::restore(const std::string& content) { try{nlohmann::json root=nlohmann::json::parse(content);objects_.clear();nextId_=1;for(const auto&e:root.at("objects")){auto object=std::make_shared<SceneObject>();object->id=e.value("id",nextId_++);object->name=e.value("name",std::string("Object_"));object->primitive=parseType(e.value("type","cube"));object->mesh=std::make_shared<Mesh>();object->mesh->data=makePrimitive(object->primitive);object->transform.position=vec3(e.value("position",nlohmann::json::array()),glm::vec3(0));object->transform.setEulerDegrees(vec3(e.value("rotation",nlohmann::json::array()),glm::vec3(0)));object->transform.scale=vec3(e.value("scale",nlohmann::json::array()),glm::vec3(1));object->visible=e.value("visible",true);object->locked=e.value("locked",false);object->material.type=parseMaterial(e.value("material","matte"));object->material.baseColor=vec3(e.value("color",nlohmann::json::array()),object->material.baseColor);object->material.metallic=e.value("metallic",0.0f);object->material.roughness=e.value("roughness",0.55f);object->material.alpha=e.value("alpha",1.0f);object->material.emissive=vec3(e.value("emissive",nlohmann::json::array()),object->material.emissive);objects_.push_back(object);nextId_=std::max(nextId_,object->id+1);}select(std::nullopt);return true;}catch(const std::exception&e){log(LogLevel::Error,std::string("Scene restore failed: ")+e.what());return false;} }
bool SceneManager::load(const std::string& path) { std::ifstream f(path);if(!f)return false;std::string content((std::istreambuf_iterator<char>(f)),{});if(!restore(content))return false;history_.clear();history_.push_back(serialize());historyIndex_=0;return true; }
void SceneManager::beginEdit(){if(editBefore_.empty())editBefore_=serialize();}
void SceneManager::endEdit(){if(editBefore_.empty())return;recordMutation(editBefore_);editBefore_.clear();}
void SceneManager::recordMutation(const std::string& before){const std::string after=serialize();if(before==after)return;if(historyIndex_+1<history_.size())history_.erase(history_.begin()+static_cast<std::ptrdiff_t>(historyIndex_+1),history_.end());history_.push_back(after);historyIndex_=history_.size()-1;}
bool SceneManager::undo(){if(!canUndo())return false;--historyIndex_;return restore(history_[historyIndex_]);}
bool SceneManager::redo(){if(!canRedo())return false;++historyIndex_;return restore(history_[historyIndex_]);}
void SceneManager::clear() { const std::string before=serialize();objects_.clear(); nextId_=1;recordMutation(before); }
}
