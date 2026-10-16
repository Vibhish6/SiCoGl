#include "HandTracking/MediaPipeHandTracker.hpp"
#include "Utils/Logger.hpp"
#include <chrono>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <nlohmann/json.hpp>
#if GESTURE3D_HAS_PYTHON_MEDIAPIPE && defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#if GESTURE3D_HAS_OPENCV
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#endif
#if GESTURE3D_HAS_MEDIAPIPE
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/formats/image.h"
#endif

namespace g3d {
namespace {
#if GESTURE3D_HAS_PYTHON_MEDIAPIPE && defined(_WIN32)
constexpr std::intptr_t kInvalidSocket = -1;

void closeBridgeSocket(std::intptr_t& socket) {
  if (socket != kInvalidSocket) {
    closesocket(static_cast<SOCKET>(socket));
    socket = kInvalidSocket;
  }
}

enum class BridgeReadResult { NoCompleteFrame, CompleteFrame, Closed, Error };

BridgeReadResult readLatestFrame(SOCKET socket, std::string& buffer, std::string& line) {
  constexpr std::size_t kMaxFrameBytes = 1024 * 1024;
  char chunk[64 * 1024];
  BridgeReadResult result = BridgeReadResult::NoCompleteFrame;

  while (true) {
    const int received = recv(socket, chunk, sizeof(chunk), 0);
    if (received > 0) {
      buffer.append(chunk, static_cast<std::size_t>(received));
      if (buffer.size() > kMaxFrameBytes * 2) return BridgeReadResult::Error;
      continue;
    }
    if (received == 0) return BridgeReadResult::Closed;

    const int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK || error == WSAEINTR) break;
    return BridgeReadResult::Error;
  }

  const std::size_t end = buffer.rfind('\n');
  if (end == std::string::npos) return result;

  const std::size_t begin = buffer.rfind('\n', end == 0 ? 0 : end - 1);
  const std::size_t frameBegin = begin == std::string::npos ? 0 : begin + 1;
  line = buffer.substr(frameBegin, end - frameBegin);
  buffer.erase(0, end + 1);
  return BridgeReadResult::CompleteFrame;
}
#endif
}

MediaPipeHandTracker::~MediaPipeHandTracker() {
#if GESTURE3D_HAS_PYTHON_MEDIAPIPE && defined(_WIN32)
  closeBridgeSocket(bridgeSocket_);
  WSACleanup();
#endif
}
bool MediaPipeHandTracker::initialize(const std::string& modelPath, int maxHands) {
#if GESTURE3D_HAS_MEDIAPIPE
  mediapipe::tasks::vision::hand_landmarker::HandLandmarkerOptions options;
  options.base_options.model_asset_path=modelPath;options.running_mode=mediapipe::tasks::vision::core::RunningMode::VIDEO;options.num_hands=maxHands;options.min_hand_detection_confidence=0.55f;options.min_hand_presence_confidence=0.55f;options.min_tracking_confidence=0.55f;
  auto created=mediapipe::tasks::vision::hand_landmarker::HandLandmarker::Create(std::make_unique<decltype(options)>(std::move(options)));if(!created.ok()){log(LogLevel::Error,"MediaPipe HandLandmarker creation failed: "+created.status().ToString());return false;}landmarker_=std::move(created.value());available_=true;timestampMs_=0;log(LogLevel::Info,"MediaPipe HandLandmarker ready");return true;
#elif GESTURE3D_HAS_PYTHON_MEDIAPIPE && defined(_WIN32)
  (void)modelPath;
  (void)maxHands;
  WSADATA wsaData{};
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    log(LogLevel::Warning, "Python MediaPipe bridge: Winsock initialization failed");
    return false;
  }
  const SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket == INVALID_SOCKET) {
    WSACleanup();
    log(LogLevel::Warning, "Python MediaPipe bridge: socket creation failed");
    return false;
  }
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(8765);
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
    closesocket(socket);
    WSACleanup();
    log(LogLevel::Warning, "Python MediaPipe bridge unavailable; start scripts/run-live.ps1");
    return false;
  }
  DWORD timeoutMs = 50;
  (void)timeoutMs;
  u_long nonBlocking = 1;
  if (ioctlsocket(socket, FIONBIO, &nonBlocking) != 0) {
    closesocket(socket);
    WSACleanup();
    log(LogLevel::Warning, "Python MediaPipe bridge: failed to configure non-blocking socket");
    return false;
  }
  bridgeSocket_ = static_cast<std::intptr_t>(socket);
  bridgeConnected_ = true;
  available_ = true;
  bridgeReceiveBuffer_.clear();
  timestampMs_ = 0;
  log(LogLevel::Info, "Python MediaPipe bridge connected");
  return true;
#else
  (void)modelPath;(void)maxHands;log(LogLevel::Warning,"MediaPipe adapter is disabled; use replay landmarks or build with MediaPipe support");return false;
#endif
}
#if GESTURE3D_HAS_MEDIAPIPE && GESTURE3D_HAS_OPENCV
bool MediaPipeHandTracker::process(const void* frame, HandFrame& output) {
  output=HandFrame{};if(!available_||!landmarker_||!frame)return false;const auto&bgr=*static_cast<const cv::Mat*>(frame);if(bgr.empty())return false;cv::Mat rgb;cv::cvtColor(bgr,rgb,cv::COLOR_BGR2RGB);
  auto imageFrame=std::make_shared<mediapipe::ImageFrame>(mediapipe::ImageFormat::SRGB,rgb.cols,rgb.rows,mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);cv::Mat destination=mediapipe::formats::MatView(imageFrame.get());rgb.copyTo(destination);mediapipe::Image image(imageFrame);const auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();timestampMs_=std::max(timestampMs_+1,static_cast<std::int64_t>(now));auto result=landmarker_->DetectForVideo(std::move(image),timestampMs_);if(!result.ok()){log(LogLevel::Warning,"MediaPipe frame failed: "+result.status().ToString());return false;}
  output.timestampMs=timestampMs_;for(std::size_t i=0;i<result->hand_landmarks.size()&&i<result->handedness.size();++i){const auto&landmarks=result->hand_landmarks[i].landmarks;if(landmarks.size()!=21||result->handedness[i].categories.empty())continue;Hand hand;hand.valid=true;const auto&category=result->handedness[i].categories.front();hand.confidence=category.score;const std::string label=category.display_name.value_or(category.category_name.value_or(""));hand.left=label=="Left";for(std::size_t j=0;j<21;++j)hand.landmarks[j]={landmarks[j].x,landmarks[j].y,landmarks[j].z};output.hands.push_back(hand);}return true;
}
#else
bool MediaPipeHandTracker::process(const void* frame, HandFrame& output){
  (void)frame;
  output=HandFrame{};
#if GESTURE3D_HAS_PYTHON_MEDIAPIPE && defined(_WIN32)
  if (!bridgeConnected_ || bridgeSocket_ == -1) return false;
  std::string line;
  const BridgeReadResult readResult = readLatestFrame(static_cast<SOCKET>(bridgeSocket_), bridgeReceiveBuffer_, line);
  if (readResult != BridgeReadResult::CompleteFrame) {
    if (readResult == BridgeReadResult::NoCompleteFrame) return false;
    bridgeConnected_ = false;
    available_ = false;
    log(LogLevel::Warning, "Python MediaPipe bridge disconnected");
    return false;
  }
  try {
    const auto json = nlohmann::json::parse(line);
    output.timestampMs = json.value("timestampMs", std::uint64_t{0});
    for (const auto& handJson : json.value("hands", nlohmann::json::array())) {
      const auto landmarks = handJson.value("landmarks", nlohmann::json::array());
      if (landmarks.size() != 21) continue;
      Hand hand;
      hand.valid = true;
      hand.left = handJson.value("left", false);
      hand.confidence = handJson.value("confidence", 0.0f);
      for (std::size_t i = 0; i < 21; ++i) {
        hand.landmarks[i] = glm::vec3(landmarks[i].size() > 0 ? landmarks[i][0].get<float>() : 0.0f, landmarks[i].size() > 1 ? landmarks[i][1].get<float>() : 0.0f, landmarks[i].size() > 2 ? landmarks[i][2].get<float>() : 0.0f);
      }
      output.hands.push_back(hand);
    }
    return true;
  } catch (const std::exception& error) {
    log(LogLevel::Warning, std::string("Python MediaPipe bridge frame failed: ") + error.what());
    return false;
  }
#else
  return false;
#endif
}
#endif
std::string MediaPipeHandTracker::status() const { return available_.load(std::memory_order_relaxed) ? (bridgeConnected_.load(std::memory_order_relaxed) ? "PYTHON" : "READY") : "DISABLED"; }
}
