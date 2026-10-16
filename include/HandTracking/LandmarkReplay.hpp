#pragma once
#include "HandTracking/HandTypes.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace g3d {
class LandmarkReplay {
 public:
  bool load(const std::string& path);
  bool next(HandFrame& frame);
  void reset() { cursor_=0; }
  bool available() const { return !frames_.empty(); }
 private:
  std::vector<HandFrame> frames_;
  std::size_t cursor_{0};
};
}
