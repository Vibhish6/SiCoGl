#include "HandTracking/CameraInput.hpp"
#include "Utils/Logger.hpp"
#if GESTURE3D_HAS_OPENCV
#include <opencv2/videoio.hpp>
#endif

namespace g3d {
bool CameraInput::open(const CameraConfig& config) {
  config_=config;
#if GESTURE3D_HAS_OPENCV
  auto* cap=new cv::VideoCapture(config.index); if(!cap->isOpened()){delete cap;log(LogLevel::Warning,"Unable to open webcam");return false;}cap->set(cv::CAP_PROP_FRAME_WIDTH,config.width);cap->set(cv::CAP_PROP_FRAME_HEIGHT,config.height);cap->set(cv::CAP_PROP_FPS,config.fps);capture_=cap;open_=true;log(LogLevel::Info,"Webcam opened");return true;
#else
  log(LogLevel::Warning,"OpenCV support is not compiled in");return false;
#endif
}
bool CameraInput::read(void* opaqueFrame) {
#if GESTURE3D_HAS_OPENCV
  if(!open_||!opaqueFrame)return false;return static_cast<cv::VideoCapture*>(capture_)->read(*static_cast<cv::Mat*>(opaqueFrame));
#else
  (void)opaqueFrame;return false;
#endif
}
void CameraInput::close(){
#if GESTURE3D_HAS_OPENCV
  delete static_cast<cv::VideoCapture*>(capture_);
#endif
  capture_=nullptr;open_=false;
}
std::string CameraInput::status() const { return open_ ? "OK" : "UNAVAILABLE"; }
}
