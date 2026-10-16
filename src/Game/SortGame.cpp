#include "Game/SortGame.hpp"
#include "Utils/Logger.hpp"
#include <algorithm>

namespace g3d {
namespace {
constexpr float floorY=0.32f;
constexpr float containerX=2.55f;
constexpr float containerHalfWidth=1.02f;
constexpr float containerHalfDepth=0.62f;
}

void SortGame::start(SceneManager& scene) {
  scene.clear();itemIds_.clear();placedIds_.clear();heldIds_.clear();velocity_.clear();score_=0;active_=true;
  const Material containerMaterial{MaterialType::Metallic,{0.08f,0.12f,0.2f},0.75f,0.25f,1.0f,{0.02f,0.04f,0.1f}};
  auto& base=scene.create(PrimitiveType::Cube,{containerX,0.12f,0.0f});base.name="Container_Base";base.transform.scale={2.5f,0.24f,1.6f};base.material=containerMaterial;base.locked=true;
  auto& left=scene.create(PrimitiveType::Cube,{containerX-containerHalfWidth,0.68f,0.0f});left.name="Container_Left_Wall";left.transform.scale={0.18f,1.15f,1.6f};left.material=containerMaterial;left.locked=true;
  auto& right=scene.create(PrimitiveType::Cube,{containerX+containerHalfWidth,0.68f,0.0f});right.name="Container_Right_Wall";right.transform.scale={0.18f,1.15f,1.6f};right.material=containerMaterial;right.locked=true;
  auto& back=scene.create(PrimitiveType::Cube,{containerX,0.68f,-containerHalfDepth});back.name="Container_Back_Wall";back.transform.scale={2.2f,1.15f,0.18f};back.material=containerMaterial;back.locked=true;
  const glm::vec3 colors[]={{0.15f,0.68f,1.0f},{0.98f,0.28f,0.25f},{0.98f,0.68f,0.12f},{0.25f,0.9f,0.5f},{0.78f,0.35f,0.95f},{0.1f,0.85f,0.85f},{0.95f,0.35f,0.55f},{0.65f,0.85f,0.2f}};
  const glm::vec3 starts[]={{-3.2f,2.4f,0.0f},{-2.35f,3.5f,0.12f},{-1.45f,2.7f,-0.1f},{-0.45f,4.0f,0.1f},{-2.75f,4.5f,-0.1f},{-1.65f,4.7f,0.08f},{-0.65f,5.2f,-0.08f},{0.15f,3.6f,0.0f}};
  for(int i=0;i<8;++i){auto& item=scene.create(i%2==0?PrimitiveType::Cube:PrimitiveType::Cylinder,starts[i]);item.name="Item_"+std::to_string(i+1);item.transform.scale={0.52f,0.52f,0.52f};item.material.baseColor=colors[i];item.material.metallic=0.2f;item.material.roughness=0.3f;itemIds_.insert(item.id);velocity_[item.id]={0.0f,0.0f,0.0f};}
  scene.select(std::nullopt);log(LogLevel::Info,"Sort & Drop game started");
}

void SortGame::update(float deltaSeconds, SceneManager& scene) {
  if(!active_)return;
  const float dt=std::clamp(deltaSeconds,0.001f,0.05f);
  for(const auto id:itemIds_){auto* item=scene.find(id);if(!item||placedIds_.count(id)||heldIds_.count(id))continue;auto& v=velocity_[id];v.y-=9.8f*dt;item->transform.position+=v*dt;item->transform.position.z=glm::clamp(item->transform.position.z,-0.5f,0.5f);
    if(item->transform.position.y<floorY){item->transform.position.y=floorY;if(std::abs(v.y)>0.25f)v.y*=-0.32f;else v.y=0.0f;}
    if(item->transform.position.x<-4.3f||item->transform.position.x>4.3f){item->transform.position.x=glm::clamp(item->transform.position.x,-4.3f,4.3f);v.x*=-0.45f;}
    const bool inside=std::abs(item->transform.position.x-containerX)<containerHalfWidth-0.18f&&std::abs(item->transform.position.z)<containerHalfDepth-0.18f&&item->transform.position.y<=1.08f;
    if(inside&&item->transform.position.y>floorY){item->transform.position.y=0.48f;v={0.0f,0.0f,0.0f};placedIds_.insert(id);item->locked=true;++score_;log(LogLevel::Info,"Sorted "+item->name);}
  }
  if(static_cast<int>(placedIds_.size())==static_cast<int>(itemIds_.size()))active_=false;
}

void SortGame::onGrab(std::uint64_t id){if(itemIds_.count(id)&&!placedIds_.count(id)){heldIds_.insert(id);velocity_[id]={0.0f,0.0f,0.0f};}}
void SortGame::onRelease(std::uint64_t id){heldIds_.erase(id);}
bool SortGame::canGrab(const SceneObject& object) const{return active_&&itemIds_.count(object.id)>0&&placedIds_.count(object.id)==0&&!object.locked;}
GameInfo SortGame::info() const {return {active_,static_cast<int>(placedIds_.size()),static_cast<int>(itemIds_.size()),score_};}
}
