cmake_minimum_required(VERSION 3.21)
project(Gesture3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(GESTURE3D_BUILD_TESTS "Build deterministic unit tests" ON)
option(GESTURE3D_WITH_OPENCV "Enable OpenCV webcam capture" ON)
option(GESTURE3D_WITH_MEDIAPIPE "Enable the MediaPipe C++ adapter when a local SDK is supplied" OFF)
option(GESTURE3D_WITH_PYTHON_MEDIAPIPE "Enable the Python MediaPipe webcam bridge" ON)
option(GESTURE3D_WITH_IMGUI "Enable Dear ImGui UI" ON)
set(MEDIAPIPE_LIBRARIES "" CACHE STRING "MediaPipe C++ libraries required when GESTURE3D_WITH_MEDIAPIPE is ON")

include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3)
FetchContent_MakeAvailable(glm glfw json)

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

if(GESTURE3D_WITH_OPENCV)
  find_package(OpenCV QUIET COMPONENTS core imgproc videoio)
endif()

set(GESTURE3D_SOURCES
  src/main.cpp
  src/Application/Application.cpp
  src/Camera/Camera.cpp
  src/Graphics/Shader.cpp
  src/Graphics/Renderer.cpp
  src/Graphics/PrimitiveFactory.cpp
  src/HandTracking/CameraInput.cpp
  src/HandTracking/MediaPipeHandTracker.cpp
  src/HandTracking/LandmarkReplay.cpp
  src/Gesture/GestureEngine.cpp
  src/Interaction/InteractionSystem.cpp
  src/Game/SortGame.cpp
  src/Scene/SceneManager.cpp
  src/Utils/Config.cpp
  src/Utils/Logger.cpp
  src/UI/UiLayer.cpp
)

add_executable(gesture3d ${GESTURE3D_SOURCES})
target_include_directories(gesture3d PRIVATE include)
target_link_libraries(gesture3d PRIVATE glfw OpenGL::GL glm::glm nlohmann_json::nlohmann_json)

target_link_libraries(gesture3d PRIVATE glad::glad)
target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_GLAD=1)

if(OpenCV_FOUND)
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_OPENCV=1)
  target_include_directories(gesture3d PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(gesture3d PRIVATE ${OpenCV_LIBS})
else()
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_OPENCV=0)
  message(WARNING "OpenCV not found: webcam capture will be disabled.")
endif()

if(GESTURE3D_WITH_MEDIAPIPE)
  if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "MediaPipe webcam processing requires OpenCV. Install OpenCV or set GESTURE3D_WITH_OPENCV=ON.")
  endif()
  if(NOT DEFINED MEDIAPIPE_ROOT)
    message(FATAL_ERROR "Set MEDIAPIPE_ROOT to a MediaPipe C++ checkout when GESTURE3D_WITH_MEDIAPIPE=ON.")
  endif()
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_MEDIAPIPE=1)
  target_include_directories(gesture3d PRIVATE "${MEDIAPIPE_ROOT}")
  if(NOT MEDIAPIPE_LIBRARIES)
    message(FATAL_ERROR "Set MEDIAPIPE_LIBRARIES to the MediaPipe Tasks C++ libraries when GESTURE3D_WITH_MEDIAPIPE=ON.")
  endif()
  target_link_libraries(gesture3d PRIVATE ${MEDIAPIPE_LIBRARIES})
else()
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_MEDIAPIPE=0)
endif()

if(GESTURE3D_WITH_PYTHON_MEDIAPIPE AND WIN32)
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_PYTHON_MEDIAPIPE=1)
  target_link_libraries(gesture3d PRIVATE ws2_32)
else()
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_PYTHON_MEDIAPIPE=0)
endif()

target_compile_definitions(gesture3d PRIVATE GESTURE3D_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets" GESTURE3D_CONFIG_DIR="${CMAKE_SOURCE_DIR}/config")

if(GESTURE3D_WITH_IMGUI)
  FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.9b)
  FetchContent_GetProperties(imgui)
  if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
  endif()
  target_sources(gesture3d PRIVATE "${imgui_SOURCE_DIR}/imgui.cpp" "${imgui_SOURCE_DIR}/imgui_draw.cpp" "${imgui_SOURCE_DIR}/imgui_tables.cpp" "${imgui_SOURCE_DIR}/imgui_widgets.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
  target_include_directories(gesture3d PRIVATE "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_IMGUI=1)
else()
  target_compile_definitions(gesture3d PRIVATE GESTURE3D_HAS_IMGUI=0)
endif()

if(MSVC)
  target_compile_options(gesture3d PRIVATE /W4 /permissive-)
else()
  target_compile_options(gesture3d PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(GESTURE3D_BUILD_TESTS)
  enable_testing()
  add_executable(gesture3d_tests tests/core_tests.cpp src/Camera/Camera.cpp src/Gesture/GestureEngine.cpp src/Interaction/InteractionSystem.cpp src/Game/SortGame.cpp src/Scene/SceneManager.cpp src/Graphics/PrimitiveFactory.cpp src/HandTracking/LandmarkReplay.cpp src/Utils/Logger.cpp)
  target_include_directories(gesture3d_tests PRIVATE include)
  target_link_libraries(gesture3d_tests PRIVATE glm::glm nlohmann_json::nlohmann_json)
  add_test(NAME gesture3d_core COMMAND gesture3d_tests)
  set_tests_properties(gesture3d_core PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

add_custom_command(TARGET gesture3d POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:gesture3d>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "$<TARGET_FILE_DIR:gesture3d>/config"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/scenes" "$<TARGET_FILE_DIR:gesture3d>/scenes")
