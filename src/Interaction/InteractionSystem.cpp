#include "Interaction/InteractionSystem.hpp"
#include "Game/SortGame.hpp"
#include <algorithm>
#include <cmath>

namespace g3d {
namespace {
constexpr float kTranslationSmoothingAlpha=0.45f;
constexpr float kScaleSmoothingAlpha=0.35f;

float shortestAngleDelta(float current, float initial) {
  constexpr float pi = 3.14159265358979323846f;
  constexpr float twoPi = 2.0f * pi;
  float delta = std::fmod(current - initial + pi, twoPi);
  if (delta < 0.0f) delta += twoPi;
  return delta - pi;
}

std::optional<glm::vec3> intersectPlaneZ(const Ray& ray,float planeZ){
  if(std::abs(ray.direction.z)<1e-5f)return std::nullopt;
  const float t=(planeZ-ray.origin.z)/ray.direction.z;
  if(!std::isfinite(t)||t<0.0f)return std::nullopt;
  const glm::vec3 point=ray.origin+ray.direction*t;
  if(!std::isfinite(point.x)||!std::isfinite(point.y)||!std::isfinite(point.z))return std::nullopt;
  return point;
}
}

void InteractionSystem::update(const GestureState& g, SceneManager& scene, const Camera& camera, int width, int height, float deltaSeconds, SortGame* game) {
  glm::vec2 safeCursor = g.cursor;
  if (!std::isfinite(safeCursor.x) || !std::isfinite(safeCursor.y)) safeCursor = {0.5f, 0.5f};
  safeCursor = glm::clamp(safeCursor, glm::vec2(0.0f), glm::vec2(1.0f));
  cursorWorld_=camera.position+camera.forward()*4.0f;
  const Ray ray=camera.rayFromScreen(safeCursor.x*width,safeCursor.y*height,width,height);
  if(const auto point=intersectPlaneZ(ray,0.0f))cursorWorld_=*point;
  if(g.pinchStarted&&mode_==InteractionMode::Create){scene.create(createType_,cursorWorld_);mode_=InteractionMode::Translate;}
  if(g.pinchStarted&&mode_!=InteractionMode::Create){
    auto hit=scene.hitTest(ray);
    if(!hit&&game&&game->active()){
      float nearest=1.3f;
      for(const auto& candidate:scene.objects())if(game->canGrab(*candidate)){const float distance=glm::distance(candidate->transform.position,cursorWorld_);if(distance<nearest){nearest=distance;hit=RayHit{candidate->id,distance,candidate->transform.position};}}
    }
    if(hit){grabbedId_=hit->objectId;if(auto* object=scene.find(grabbedId_);object&&(!game||!game->active()||game->canGrab(*object))){scene.select(grabbedId_);grabPlaneZ_=(game&&game->active())?0.0f:object->transform.position.z;const glm::vec3 grabPoint=intersectPlaneZ(ray,grabPlaneZ_).value_or(cursorWorld_);grabOffset_=object->transform.position-grabPoint;smoothedGrabPosition_=object->transform.position;grabPositionInitialized_=true;initialPosition_=object->transform.position;manipulating_=true;initialTwoHandDistance_=g.handDistance;initialScale_=object->transform.scale;initialOrientation_=object->transform.orientation;initialHandAngle_=g.handAngle;initialPrimaryRoll_=g.primaryRoll;scene.beginEdit();if(game)game->onGrab(object->id);}}
  }
  if(manipulating_&&grabbedId_){
    auto* object=scene.find(grabbedId_);
    if(!object){manipulating_=false;grabPositionInitialized_=false;grabbedId_=0;}
    else if(game&&game->active()&&!game->canGrab(*object)){scene.endEdit();manipulating_=false;grabPositionInitialized_=false;grabbedId_=0;}
    else if(g.gesture==Gesture::TwoHandPinch){
      // A second hand can enter after the initial pinch selected the object.
      // Establish the scale/angle baseline at that transition to avoid a jump.
      if (!std::isfinite(g.handDistance) || !std::isfinite(g.handAngle) || g.handDistance <= 0.01f) {
        twoHandActive_ = false;
        smoothedTwoHandDistance_ = 0.0f;
      } else if (g.twoHandPinchStarted || !twoHandActive_) {
        initialTwoHandDistance_ = g.handDistance;
        initialScale_ = object->transform.scale;
        initialOrientation_ = object->transform.orientation;
        initialHandAngle_ = g.handAngle;
        smoothedTwoHandDistance_ = g.handDistance;
        twoHandActive_ = g.handDistance > 0.01f;
      } else {
        const float dt = glm::clamp(deltaSeconds, 0.0f, 0.1f);
        const float alpha = 1.0f - std::pow(1.0f - kScaleSmoothingAlpha, dt * 60.0f);
        smoothedTwoHandDistance_ = glm::mix(smoothedTwoHandDistance_, g.handDistance, glm::clamp(alpha, 0.0f, 1.0f));
        object->transform.scale = glm::clamp(initialScale_ * (smoothedTwoHandDistance_ / initialTwoHandDistance_), glm::vec3(.1f), glm::vec3(5.0f));
        const float angleDelta = shortestAngleDelta(g.handAngle, initialHandAngle_);
        object->transform.orientation = glm::normalize(glm::angleAxis(angleDelta, glm::vec3(0, 1, 0)) * initialOrientation_);
      }
    }
    else if(g.gesture==Gesture::Pinch&&mode_==InteractionMode::Rotate){
      twoHandActive_ = false;
      smoothedTwoHandDistance_ = 0.0f;
      const float angleDelta = shortestAngleDelta(g.primaryRoll, initialPrimaryRoll_);
      object->transform.orientation = glm::normalize(glm::angleAxis(angleDelta, glm::vec3(0, 1, 0)) * initialOrientation_);
    }
    else if(g.gesture==Gesture::Pinch&&mode_!=InteractionMode::Scale){
      twoHandActive_ = false;
      smoothedTwoHandDistance_ = 0.0f;
      if(const auto grabPoint=intersectPlaneZ(ray,grabPlaneZ_)){
        const glm::vec3 target=*grabPoint+grabOffset_;
        if(!grabPositionInitialized_){smoothedGrabPosition_=target;grabPositionInitialized_=true;}
        else{const float dt=glm::clamp(deltaSeconds,0.0f,0.1f);const float alpha=1.0f-std::pow(1.0f-kTranslationSmoothingAlpha,dt*60.0f);smoothedGrabPosition_=glm::mix(smoothedGrabPosition_,target,glm::clamp(alpha,0.0f,1.0f));}
        object->transform.position=smoothedGrabPosition_;
        if(game&&game->active()){object->transform.position.z=0.0f;object->transform.position.y=std::max(object->transform.position.y,0.34f);}
        if(axisLock_)object->transform.position[*axisLock_]=initialPosition_[*axisLock_];
      }
    }
  }
  if(g.twoHandPinchReleased)twoHandActive_=false;
  if(g.pinchReleased){if(manipulating_)scene.endEdit();if(game&&grabbedId_)game->onRelease(grabbedId_);manipulating_=false;grabPositionInitialized_=false;twoHandActive_=false;smoothedTwoHandDistance_=0.0f;grabbedId_=0;}
  if(g.deleteConfirmed){if(auto* object=scene.selected()){const auto id=object->id;scene.remove(id);if(grabbedId_==id){grabbedId_=0;manipulating_=false;}}}
}
}
