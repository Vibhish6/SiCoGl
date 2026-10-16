#pragma once
#include <string>

namespace g3d {
struct CameraConfig { int index{0}; int width{640}; int height{480}; int fps{30}; };
class CameraInput {
 public:
  bool open(const CameraConfig& config);
  bool read(void* opaqueFrame);
  void close();
  bool isOpen() const { return open_; }
  std::string status() const;
 private:
  bool open_{false};
  CameraConfig config_;
  void* capture_{nullptr};
};
}
