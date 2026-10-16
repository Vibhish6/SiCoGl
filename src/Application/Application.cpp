#include "Application/Application.hpp"
#include "Utils/Logger.hpp"
#if GESTURE3D_HAS_GLAD
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <optional>
#include <string>
#include <thread>
#if GESTURE3D_HAS_OPENCV
#include <opencv2/core.hpp>
#endif

namespace g3d {
bool Application::initialize(const AppConfig& config) {
  config_=config;
  if(!glfwInit()){log(LogLevel::Critical,"GLFW initialization failed");return false;}
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
  window_=glfwCreateWindow(width_,height_,"Gesture3D | OpenGL Hand Modeling",nullptr,nullptr);if(!window_){log(LogLevel::Critical,"Window creation failed");glfwTerminate();return false;}
  glfwMakeContextCurrent(window_);glfwSwapInterval(1);glfwSetWindowUserPointer(window_,this);glfwSetFramebufferSizeCallback(window_,framebufferCallback);
#if GESTURE3D_HAS_GLAD
  if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){log(LogLevel::Critical,"OpenGL function loading failed");return false;}
  log(LogLevel::Info,std::string("OpenGL ")+reinterpret_cast<const char*>(glGetString(GL_VERSION)));
#endif
  if(!renderer_.initialize(GESTURE3D_ASSET_DIR))return false;renderer_.resize(width_,height_);ui_.initialize(window_);
  if(!scene_.load(config_.demoScene)){scene_.create(PrimitiveType::Cube,{-1,0.5f,0});scene_.create(PrimitiveType::Sphere,{1,0.65f,0});scene_.create(PrimitiveType::Torus,{0,1.2f,-1});scene_.create(PrimitiveType::Cylinder,{2,0.6f,-1});}
  game_.start(scene_);
  if(!config_.replayFile.empty())replay_.load(config_.replayFile);std::string model=config_.handLandmarkerModel;if(!model.empty()&&model.find(':')!=std::string::npos){}else if(!model.empty()&&model.rfind("assets/",0)==0)model=std::string(GESTURE3D_ASSET_DIR)+model.substr(6);const bool trackerReady=tracker_.initialize(model);if(trackerReady&&!tracker_.usesExternalCamera())cameraInput_.open(config_.camera);return true;
}
void Application::framebufferCallback(GLFWwindow* window,int width,int height){if(auto*self=static_cast<Application*>(glfwGetWindowUserPointer(window))){self->width_=width;self->height_=height;self->renderer_.resize(width,height);}}
void Application::processKeyboard(float dt) {
  const float speed=3.0f*dt;if(glfwGetKey(window_,GLFW_KEY_W)==GLFW_PRESS)camera_.position+=camera_.forward()*speed;if(glfwGetKey(window_,GLFW_KEY_S)==GLFW_PRESS)camera_.position-=camera_.forward()*speed;
  const glm::vec3 right=glm::normalize(glm::cross(camera_.forward(),glm::vec3(0,1,0)));if(glfwGetKey(window_,GLFW_KEY_D)==GLFW_PRESS)camera_.position+=right*speed;if(glfwGetKey(window_,GLFW_KEY_A)==GLFW_PRESS)camera_.position-=right*speed;
  static bool previousDelete=false;const bool deleteDown=glfwGetKey(window_,GLFW_KEY_DELETE)==GLFW_PRESS;if(deleteDown&&!previousDelete)if(auto*o=scene_.selected())scene_.remove(o->id);previousDelete=deleteDown;
  static bool previousSave=false;const bool saveDown=glfwGetKey(window_,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS&&glfwGetKey(window_,GLFW_KEY_S)==GLFW_PRESS;if(saveDown&&!previousSave)scene_.save("scenes/user_scene.json");previousSave=saveDown;
  if(glfwGetKey(window_,GLFW_KEY_R)==GLFW_PRESS)interaction_.setMode(InteractionMode::Rotate);else if(glfwGetKey(window_,GLFW_KEY_G)==GLFW_PRESS)interaction_.setMode(InteractionMode::Scale);else if(glfwGetKey(window_,GLFW_KEY_T)==GLFW_PRESS)interaction_.setMode(InteractionMode::Translate);else if(glfwGetKey(window_,GLFW_KEY_P)==GLFW_PRESS){interaction_.setCreateType(PrimitiveType::Cube);interaction_.setMode(InteractionMode::Create);}
  if(glfwGetKey(window_,GLFW_KEY_X)==GLFW_PRESS)interaction_.setAxisLock(0);else if(glfwGetKey(window_,GLFW_KEY_Y)==GLFW_PRESS)interaction_.setAxisLock(1);else if(glfwGetKey(window_,GLFW_KEY_Z)==GLFW_PRESS)interaction_.setAxisLock(2);else interaction_.setAxisLock(std::nullopt);
  static bool previousKeys[6]{};const int keys[6]={GLFW_KEY_1,GLFW_KEY_2,GLFW_KEY_3,GLFW_KEY_4,GLFW_KEY_5,GLFW_KEY_6};const PrimitiveType types[6]={PrimitiveType::Cube,PrimitiveType::Sphere,PrimitiveType::Cylinder,PrimitiveType::Cone,PrimitiveType::Torus,PrimitiveType::Plane};for(int i=0;i<6;++i){const bool down=glfwGetKey(window_,keys[i])==GLFW_PRESS;if(down&&!previousKeys[i]){scene_.create(types[i],interaction_.cursorWorld());}previousKeys[i]=down;}
  static bool previousDuplicate=false;const bool duplicateDown=glfwGetKey(window_,GLFW_KEY_C)==GLFW_PRESS;if(duplicateDown&&!previousDuplicate)if(auto*o=scene_.selected())scene_.duplicate(o->id);previousDuplicate=duplicateDown;
  static bool previousUndo=false;const bool undoDown=glfwGetKey(window_,GLFW_KEY_Z)==GLFW_PRESS&&glfwGetKey(window_,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS;if(undoDown&&!previousUndo)scene_.undo();previousUndo=undoDown;
  static bool previousRedo=false;const bool redoDown=glfwGetKey(window_,GLFW_KEY_Y)==GLFW_PRESS&&glfwGetKey(window_,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS;if(redoDown&&!previousRedo)scene_.redo();previousRedo=redoDown;
  static bool previousMaterial=false;const bool materialDown=glfwGetKey(window_,GLFW_KEY_M)==GLFW_PRESS;if(materialDown&&!previousMaterial)if(auto*o=scene_.selected()){Material material=o->material;material.baseColor=glm::vec3(material.baseColor.b,material.baseColor.r,material.baseColor.g);material.metallic=material.metallic>0.5f?0.0f:0.8f;scene_.setMaterial(o->id,material);}previousMaterial=materialDown;
  static bool previousGame=false;const bool gameDown=glfwGetKey(window_,GLFW_KEY_N)==GLFW_PRESS;if(gameDown&&!previousGame)game_.start(scene_);previousGame=gameDown;
}
void Application::trackingLoop(){
  while(trackingRunning_.load(std::memory_order_relaxed)){
    HandFrame frame;bool processed=false;
#if GESTURE3D_HAS_OPENCV
    if(tracker_.usesExternalCamera())processed=tracker_.process(nullptr,frame);else{cv::Mat cameraFrame;if(cameraInput_.read(&cameraFrame))processed=tracker_.process(&cameraFrame,frame);}
#else
    if(tracker_.usesExternalCamera())processed=tracker_.process(nullptr,frame);
#endif
    if(processed){std::lock_guard<std::mutex> lock(trackingMutex_);latestHandFrame_=std::move(frame);++trackingSequence_;}
    else std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}
void Application::stopTracking(){if(!trackingRunning_.exchange(false,std::memory_order_relaxed))return;if(trackingThread_.joinable())trackingThread_.join();}
int Application::run(){if(replay_.available()==false&&tracker_.available()){trackingRunning_.store(true,std::memory_order_relaxed);trackingThread_=std::thread(&Application::trackingLoop,this);}auto last=std::chrono::steady_clock::now();float fps=0.0f;float trackingAgeSeconds=0.0f;bool trackingExpired=false;std::uint64_t consumedTrackingSequence=0;GestureState lastGesture;while(window_&&!glfwWindowShouldClose(window_)){const auto now=std::chrono::steady_clock::now();const float dt=std::chrono::duration<float>(now-last).count();last=now;if(dt>0)fps=0.9f*fps+0.1f/dt;glfwPollEvents();processKeyboard(dt);
    HandFrame handFrame;bool newTrackingFrame=false;
#if GESTURE3D_HAS_OPENCV
    if(replay_.available()){replay_.next(handFrame);newTrackingFrame=true;}else if(tracker_.available()){std::lock_guard<std::mutex> lock(trackingMutex_);if(trackingSequence_!=consumedTrackingSequence){handFrame=latestHandFrame_;consumedTrackingSequence=trackingSequence_;newTrackingFrame=true;}}
#else
    if(replay_.available()){replay_.next(handFrame);newTrackingFrame=true;}else if(tracker_.available()){std::lock_guard<std::mutex> lock(trackingMutex_);if(trackingSequence_!=consumedTrackingSequence){handFrame=latestHandFrame_;consumedTrackingSequence=trackingSequence_;newTrackingFrame=true;}}
#endif
    if(newTrackingFrame){lastGesture=gestures_.update(handFrame);trackingAgeSeconds=0.0f;trackingExpired=false;}else{trackingAgeSeconds+=std::max(dt,0.0f);}GestureState gesture=lastGesture;if(!newTrackingFrame){gesture.pinchStarted=false;gesture.pinchReleased=false;gesture.twoHandPinchStarted=false;gesture.twoHandPinchReleased=false;gesture.deleteConfirmed=false;if(trackingAgeSeconds>0.25f&&!trackingExpired){const bool hadPinch=lastGesture.gesture==Gesture::Pinch||lastGesture.gesture==Gesture::TwoHandPinch;const bool hadTwoHandPinch=lastGesture.gesture==Gesture::TwoHandPinch;gesture.gesture=Gesture::None;gesture.handCount=0;gesture.confidence=0.0f;gesture.hasTwoHands=false;gesture.handDistance=0.0f;gesture.pinchReleased=hadPinch;gesture.twoHandPinchReleased=hadTwoHandPinch;lastGesture=gesture;gestures_.reset();trackingExpired=true;}}static bool mouseActive=false;double mouseX=0,mouseY=0;glfwGetCursorPos(window_,&mouseX,&mouseY);if(gesture.gesture==Gesture::None){const bool mouseDown=glfwGetMouseButton(window_,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;gesture.cursor={static_cast<float>(mouseX/std::max(1,width_)),static_cast<float>(mouseY/std::max(1,height_))};if(mouseDown){gesture.gesture=Gesture::Pinch;gesture.confidence=1.0f;gesture.pinchStarted=!mouseActive;}else if(mouseActive){gesture.gesture=Gesture::Point;gesture.pinchReleased=true;}mouseActive=mouseDown;}
    static Gesture previousGesture=Gesture::None;if(gesture.gesture==Gesture::ThreeFinger&&previousGesture!=Gesture::ThreeFinger)if(auto*o=scene_.selected())scene_.duplicate(o->id);previousGesture=gesture.gesture;interaction_.update(gesture,scene_,camera_,width_,height_,dt,&game_);game_.update(dt,scene_);renderer_.render(scene_,camera_,interaction_.cursorWorld(),config_.showGrid);
    std::string title="Gesture3D | Sort & Drop | "+std::to_string(game_.info().placed)+"/"+std::to_string(game_.info().total)+" sorted | FPS: "+std::to_string(static_cast<int>(fps))+" | Tracking: "+(replay_.available()?"REPLAY":tracker_.status());glfwSetWindowTitle(window_,title.c_str());ui_.render(scene_,game_,gesture,replay_.available()?"REPLAY":tracker_.status(),fps);glfwSwapBuffers(window_);
  }return 0;}
void Application::shutdown(){stopTracking();ui_.shutdown();renderer_.shutdown();cameraInput_.close();if(window_){glfwDestroyWindow(window_);window_=nullptr;}glfwTerminate();}
}
