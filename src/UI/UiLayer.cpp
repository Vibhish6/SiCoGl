#include "UI/UiLayer.hpp"
#include "Utils/Logger.hpp"
#if GESTURE3D_HAS_IMGUI
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#endif

namespace g3d {
bool UiLayer::initialize(GLFWwindow* window) {
#if GESTURE3D_HAS_IMGUI
  IMGUI_CHECKVERSION();ImGui::CreateContext();ImGuiIO& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;ImGui::StyleColorsDark();ImGuiStyle& style=ImGui::GetStyle();style.WindowRounding=12.0f;style.ChildRounding=8.0f;style.FrameRounding=6.0f;style.GrabRounding=6.0f;style.WindowBorderSize=0.0f;style.FrameBorderSize=0.0f;style.WindowPadding={16.0f,14.0f};style.ItemSpacing={10.0f,8.0f};style.Colors[ImGuiCol_WindowBg]=ImVec4(0.035f,0.045f,0.075f,0.94f);style.Colors[ImGuiCol_ChildBg]=ImVec4(0.055f,0.07f,0.11f,0.85f);style.Colors[ImGuiCol_Button]=ImVec4(0.12f,0.38f,0.62f,1.0f);style.Colors[ImGuiCol_ButtonHovered]=ImVec4(0.18f,0.52f,0.78f,1.0f);style.Colors[ImGuiCol_Header]=ImVec4(0.10f,0.28f,0.46f,1.0f);if(!ImGui_ImplGlfw_InitForOpenGL(window,true)||!ImGui_ImplOpenGL3_Init("#version 330")){log(LogLevel::Error,"Dear ImGui initialization failed");return false;}initialized_=true;return true;
#else
  (void)window;return false;
#endif
}
void UiLayer::render(SceneManager& scene,SortGame& game,const GestureState& gesture,const std::string& trackingStatus,float fps) {
#if GESTURE3D_HAS_IMGUI
  if(!initialized_)return;ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();ImGui::SetNextWindowPos(ImVec2(22,22),ImGuiCond_FirstUseEver);ImGui::SetNextWindowSize(ImVec2(340,0),ImGuiCond_FirstUseEver);ImGui::Begin("GESTURE3D  //  SORT LAB");ImGui::TextColored(ImVec4(0.35f,0.85f,1.0f,1.0f),"2.5D PHYSICS PLAYGROUND");ImGui::TextDisabled("Pick up objects. Drop them in the container.");ImGui::Separator();const GameInfo info=game.info();if(ImGui::Button("NEW SORTING RUN",ImVec2(-1,34)))game.start(scene);ImGui::Spacing();ImGui::Text("SCORE  %04d",info.score);ImGui::SameLine(180);ImGui::Text("ITEMS  %d/%d",info.placed,info.total);const float progress=info.total>0?static_cast<float>(info.placed)/info.total:0.0f;ImGui::ProgressBar(progress,ImVec2(-1,18));if(!info.active&&info.total>0)ImGui::TextColored(ImVec4(0.3f,1.0f,0.55f,1.0f),"RUN COMPLETE  //  PRESS N TO PLAY AGAIN");else ImGui::TextWrapped("Pinch an item to pick it up. Move it above the container and release.");ImGui::Spacing();if(ImGui::CollapsingHeader("LIVE TRACKING",ImGuiTreeNodeFlags_DefaultOpen)){ImGui::Text("STATUS");ImGui::SameLine(112);ImGui::TextColored(trackingStatus=="PYTHON"?ImVec4(0.3f,1.0f,0.55f,1.0f):ImVec4(1.0f,0.7f,0.25f,1.0f),"%s",trackingStatus.c_str());ImGui::Text("GESTURE");ImGui::SameLine(112);ImGui::TextColored(ImVec4(0.4f,0.85f,1.0f,1.0f),"%s",GestureEngine::toString(gesture.gesture).c_str());ImGui::Text("CONFIDENCE");ImGui::SameLine(112);ImGui::Text("%.0f%%",gesture.confidence*100.0f);ImGui::Text("HANDS");ImGui::SameLine(112);ImGui::Text("%d",gesture.handCount);ImGui::Text("PERFORMANCE");ImGui::SameLine(112);ImGui::Text("%.1f FPS",fps);}
  if(ImGui::CollapsingHeader("ITEMS",ImGuiTreeNodeFlags_DefaultOpen)){for(const auto& object:scene.objects()){if(object->locked)continue;const bool selected=object->selected;if(ImGui::Selectable(object->name.c_str(),selected))scene.select(object->id);}}
  if(auto* object=scene.selected()){if(ImGui::CollapsingHeader("SELECTED OBJECT",ImGuiTreeNodeFlags_DefaultOpen)){ImGui::TextColored(ImVec4(0.4f,0.85f,1.0f,1.0f),"%s",object->name.c_str());ImGui::TextDisabled("Physics: %s",object->locked?"SETTLED":"DYNAMIC");}}
  ImGui::Separator();ImGui::TextDisabled("N  new run   |   W/A/S/D  camera");ImGui::TextDisabled("Pinch = pickup/drop   |   Two-hand = scale");ImGui::End();ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
  (void)scene;(void)game;(void)gesture;(void)trackingStatus;(void)fps;
#endif
}
void UiLayer::shutdown(){
#if GESTURE3D_HAS_IMGUI
  if(initialized_){ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();initialized_=false;}
#endif
}
}
