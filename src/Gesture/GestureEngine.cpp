#include "Gesture/GestureEngine.hpp"
#include <algorithm>
#include <cmath>

namespace g3d {
GestureEngine::GestureEngine(GestureConfig config):config_(config){}
namespace {
bool finiteHand(const Hand& hand) {
  if (!hand.valid || !std::isfinite(hand.confidence)) return false;
  for (const auto& landmark : hand.landmarks) {
    if (!std::isfinite(landmark.x) || !std::isfinite(landmark.y) || !std::isfinite(landmark.z)) return false;
  }
  return true;
}

bool extended(const Hand& h, int mcp, int pip, int dip, int tip) {
  const glm::vec3 proximal = h.landmarks[pip] - h.landmarks[mcp];
  const glm::vec3 intermediate = h.landmarks[dip] - h.landmarks[pip];
  const glm::vec3 distal = h.landmarks[tip] - h.landmarks[dip];
  const float proximalLength = glm::length(proximal);
  const float intermediateLength = glm::length(intermediate);
  const float distalLength = glm::length(distal);
  if (proximalLength < 1e-5f || intermediateLength < 1e-5f || distalLength < 1e-5f) return false;

  const float tipDistance = glm::distance(h.landmarks[tip], h.landmarks[0]);
  const float pipDistance = glm::distance(h.landmarks[pip], h.landmarks[0]);
  const float proximalAlignment = glm::dot(proximal, intermediate) / (proximalLength * intermediateLength);
  const float distalAlignment = glm::dot(intermediate, distal) / (intermediateLength * distalLength);
  return tipDistance > pipDistance * 1.08f && proximalAlignment > -0.2f && distalAlignment > -0.2f;
}

float normalizedPinchDistance(const Hand& hand) {
  return hand.pinchDistance() / std::max(hand.sizeEstimate(), 0.001f);
}

bool pinched(const Hand& hand, float threshold) {
  return finiteHand(hand) && std::isfinite(normalizedPinchDistance(hand)) && normalizedPinchDistance(hand) < threshold;
}
}
Gesture GestureEngine::classify(const Hand& h) const {
  if(!finiteHand(h)||h.confidence<config_.minimumConfidence)return Gesture::None;
  const bool index=extended(h,5,6,7,8), middle=extended(h,9,10,11,12), ring=extended(h,13,14,15,16), pinky=extended(h,17,18,19,20);
  const float normalizedPinch=normalizedPinchDistance(h);
  if(normalizedPinch<config_.pinchThreshold)return Gesture::Pinch;
  if(index&&!middle&&!ring&&!pinky)return Gesture::Point;
  if(index&&middle&&ring&&pinky)return Gesture::OpenPalm;
  if(!index&&!middle&&!ring&&!pinky)return Gesture::Fist;
  if(index&&middle&&!ring&&!pinky)return Gesture::TwoFinger;
  if(index&&middle&&ring&&!pinky)return Gesture::ThreeFinger;
  return Gesture::None;
}
GestureState GestureEngine::update(const HandFrame& frame) {
  GestureState out;out.handCount=static_cast<int>(frame.hands.size()); if(deleteCooldown_>0)--deleteCooldown_; if(frame.hands.empty()){out.pinchReleased=pinchActive_;out.twoHandPinchReleased=twoHandPinchActive_;out.cursor=smoothedCursor_;pinchActive_=false;twoHandPinchActive_=false;fistFrames_=0;cursorInitialized_=false;previousGesture_=Gesture::None;return out;}
  const Hand* primary=nullptr; for(const auto&h:frame.hands)if(finiteHand(h)&&(!primary||h.confidence>primary->confidence))primary=&h;
  if(!primary){out.pinchReleased=pinchActive_;out.twoHandPinchReleased=twoHandPinchActive_;out.cursor=smoothedCursor_;pinchActive_=false;twoHandPinchActive_=false;fistFrames_=0;return out;}
  const glm::vec3 tip=primary->landmarks[8]; const glm::vec2 cursor{glm::clamp(1.0f-tip.x,0.02f,0.98f),glm::clamp(tip.y,0.02f,0.98f)}; if(!cursorInitialized_)smoothedCursor_=cursor;else{const float movement=glm::length(cursor-smoothedCursor_);const float alpha=glm::clamp(config_.smoothingAlpha+movement*0.75f,config_.smoothingAlpha,0.65f);smoothedCursor_=alpha*cursor+(1.0f-alpha)*smoothedCursor_;}cursorInitialized_=true;out.cursor=smoothedCursor_;
  out.gesture=classify(*primary);out.confidence=primary->confidence;out.primaryRoll=std::atan2(primary->landmarks[9].y-primary->landmarks[0].y,primary->landmarks[9].x-primary->landmarks[0].x);const bool pinch=pinched(*primary,pinchActive_?config_.pinchReleaseThreshold:config_.pinchThreshold);if(pinch)out.gesture=Gesture::Pinch;
  out.pinchStarted=pinch&&!pinchActive_;out.pinchReleased=!pinch&&pinchActive_;pinchActive_=pinch;
  if(out.gesture==Gesture::Fist)++fistFrames_;else fistFrames_=0; if(fistFrames_>=config_.holdFrames&&deleteCooldown_==0){out.deleteConfirmed=true;deleteCooldown_=config_.deleteCooldownFrames;fistFrames_=0;}
  if(frame.hands.size()>=2){
    const Hand* first=&frame.hands[0];
    const Hand* second=&frame.hands[1];
    // MediaPipe can reorder detections between frames. Prefer handedness so a
    // left/right swap cannot add π to the twist angle.
    if (frame.hands[0].left != frame.hands[1].left) {
      if (frame.hands[0].left) { first=&frame.hands[0]; second=&frame.hands[1]; }
      else { first=&frame.hands[1]; second=&frame.hands[0]; }
    }
    if (finiteHand(*first) && finiteHand(*second)) {
      out.hasTwoHands=true;out.handVector=second->palmCenter()-first->palmCenter();out.handDistance=glm::length(out.handVector);out.handAngle=std::atan2(out.handVector.y,out.handVector.x);
      const float twoHandThreshold=twoHandPinchActive_?config_.pinchReleaseThreshold:config_.pinchThreshold;
      const bool twoHandPinching=pinched(*first,twoHandThreshold)&&pinched(*second,twoHandThreshold)&&std::isfinite(out.handDistance)&&out.handDistance>0.01f;
      out.twoHandPinchStarted=twoHandPinching&&!twoHandPinchActive_;out.twoHandPinchReleased=!twoHandPinching&&twoHandPinchActive_;twoHandPinchActive_=twoHandPinching;
      if(twoHandPinching)out.gesture=Gesture::TwoHandPinch;
    } else {
      out.twoHandPinchReleased=twoHandPinchActive_;twoHandPinchActive_=false;
    }
  } else {
    out.twoHandPinchReleased=twoHandPinchActive_;
    twoHandPinchActive_=false;
  }
  previousGesture_=out.gesture;
  return out;
}
void GestureEngine::reset(){pinchActive_=false;twoHandPinchActive_=false;fistFrames_=0;smoothedCursor_={0.5f,0.5f};cursorInitialized_=false;previousGesture_=Gesture::None;}
std::string GestureEngine::toString(Gesture g){switch(g){case Gesture::Point:return"POINT";case Gesture::Pinch:return"PINCH";case Gesture::OpenPalm:return"OPEN PALM";case Gesture::Fist:return"FIST";case Gesture::TwoHandPinch:return"TWO-HAND PINCH";case Gesture::TwoFinger:return"TWO FINGER";case Gesture::ThreeFinger:return"THREE FINGER";default:return"NO HAND";}}
}
