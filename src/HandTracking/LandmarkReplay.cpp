#include "HandTracking/LandmarkReplay.hpp"
#include "Utils/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

namespace g3d {
bool LandmarkReplay::load(const std::string& path) {
  frames_.clear();cursor_=0;std::ifstream file(path);if(!file){log(LogLevel::Error,"Replay file not found: "+path);return false;}
  try{nlohmann::json root;file>>root;for(const auto&jf:root.at("frames")){HandFrame frame;frame.timestampMs=jf.value("timestampMs",0ULL);for(const auto&jh:jf.value("hands",nlohmann::json::array())){Hand hand;hand.valid=true;hand.left=jh.value("left",false);hand.confidence=jh.value("confidence",1.0f);const auto&points=jh.at("landmarks");if(!points.is_array()||points.size()!=21)continue;for(std::size_t i=0;i<21;++i){if(!points[i].is_array()||points[i].size()!=3){hand.valid=false;break;}hand.landmarks[i]={points[i][0].get<float>(),points[i][1].get<float>(),points[i][2].get<float>()};}if(hand.valid)frame.hands.push_back(hand);}frames_.push_back(frame);}return !frames_.empty();}catch(const std::exception&e){log(LogLevel::Error,std::string("Replay parse failed: ")+e.what());return false;}
}
bool LandmarkReplay::next(HandFrame& frame){if(frames_.empty())return false;frame=frames_[cursor_];cursor_=(cursor_+1)%frames_.size();return true;}
}
