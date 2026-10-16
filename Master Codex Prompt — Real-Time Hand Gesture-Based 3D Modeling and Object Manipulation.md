You are the lead software engineer and Computer Graphics engineer responsible for building a complete, production-quality academic project titled:

# Real-Time Hand Gesture-Based 3D Modeling and Object Manipulation Using OpenGL

## 1. PROJECT OBJECTIVE

Build a complete interactive 3D modeling environment in which a user can create, select, manipulate, transform, duplicate, modify, and delete 3D objects using natural hand gestures captured through a webcam.

The application MUST use:

- OpenGL for the actual 3D rendering pipeline
- OpenCV for webcam/video capture and image processing
- MediaPipe for real-time hand landmark detection and tracking

Do NOT replace OpenGL with Unity, Unreal Engine, Godot, Three.js, Blender as the rendering engine, or any other 3D/game engine.

The final application should feel like a small futuristic touchless CAD/3D modeling application rather than a simple gesture demo.

The project must clearly demonstrate Computer Graphics concepts including:

- 3D coordinate systems
- Model transformations
- Translation
- Rotation
- Scaling
- Model/View/Projection matrices
- Perspective projection
- Camera transformations
- Lighting
- Materials
- Depth testing
- Object selection
- Multiple objects
- Real-time rendering
- Scene management
- Rendering modes
- Coordinate conversion
- Interaction between a real-world camera and a virtual 3D scene

The implementation must be suitable for a final-year engineering/Computer Graphics project demonstration.

---

# 2. PRIORITY ORDER

Prioritize the project in this order:

1. Correct OpenGL rendering
2. Reliable hand tracking
3. Reliable gesture recognition
4. Natural object manipulation
5. Stable multi-object scene management
6. Professional UI
7. Advanced rendering effects
8. Optional experimental features

Do NOT sacrifice reliability merely to add more features.

Every feature must be implemented properly rather than being a placeholder.

---

# 3. TARGET PLATFORM

Primary target:

- Windows 10/11
- Webcam
- NVIDIA/AMD/Intel GPU supported where possible

Prefer a setup that can be installed and run by another student without requiring complex manual configuration.

Document exact requirements.

---

# 4. RECOMMENDED TECHNOLOGY STACK

Use:

- C++17 or newer
- OpenGL 3.3 Core Profile or newer
- GLFW for window/context/input handling
- GLAD for OpenGL function loading
- GLM for mathematics
- OpenCV for webcam capture
- MediaPipe for hand tracking
- CMake for project/build management

Use other small libraries only when they provide a clear benefit.

Do not introduce unnecessary dependencies.

---

# 5. ARCHITECTURE

Use a modular architecture.

Suggested high-level structure:

```text
Application
│
├── CameraInput
│   └── OpenCV webcam capture
│
├── HandTracking
│   └── MediaPipe landmark detection
│
├── GestureRecognition
│   ├── pinch
│   ├── grab
│   ├── open palm
│   ├── point
│   ├── fist
│   ├── two-hand gestures
│   └── gesture state machine
│
├── InteractionSystem
│   ├── selection
│   ├── object grabbing
│   ├── translation
│   ├── rotation
│   ├── scaling
│   ├── creation
│   ├── deletion
│   └── duplication
│
├── Scene
│   ├── SceneManager
│   ├── SceneObject
│   ├── Transform
│   ├── Material
│   ├── Mesh
│   └── Object selection
│
├── Renderer
│   ├── OpenGL initialization
│   ├── shaders
│   ├── camera
│   ├── lighting
│   ├── materials
│   ├── depth testing
│   ├── shadows
│   └── post/visual effects where appropriate
│
├── UI
│   ├── mode indicators
│   ├── gesture status
│   ├── object information
│   ├── material controls
│   └── help overlay
│
├── Input
│   └── keyboard/mouse only for debugging and fallback
│
├── Serialization
│   ├── save scene
│   └── load scene
│
└── Utilities
    ├── logging
    ├── configuration
    ├── timing
    └── diagnostics
```

The system must maintain a clean separation between:

```text
Hand landmarks
        ↓
Gesture interpretation
        ↓
Interaction command
        ↓
Scene/object state modification
        ↓
OpenGL rendering
```

Do not directly manipulate OpenGL objects from the MediaPipe callback.

---

# 6. PROJECT DIRECTORY STRUCTURE

Create an organized directory structure similar to:

```text
Gesture3D/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── assets/
│   ├── shaders/
│   │   ├── basic.vert
│   │   ├── basic.frag
│   │   ├── phong.vert
│   │   ├── phong.frag
│   │   ├── shadow.vert
│   │   └── shadow.frag
│   │
│   ├── textures/
│   └── materials/
│
├── config/
│   └── config.json
│
├── include/
│   ├── Application/
│   ├── Camera/
│   ├── Graphics/
│   ├── HandTracking/
│   ├── Gesture/
│   ├── Interaction/
│   ├── Scene/
│   ├── UI/
│   └── Utils/
│
├── src/
│   ├── Application/
│   ├── Camera/
│   ├── Graphics/
│   ├── HandTracking/
│   ├── Gesture/
│   ├── Interaction/
│   ├── Scene/
│   ├── UI/
│   └── Utils/
│
├── tests/
│   ├── gesture_tests/
│   ├── math_tests/
│   └── scene_tests/
│
├── docs/
│   ├── architecture.md
│   ├── gestures.md
│   ├── graphics_pipeline.md
│   ├── build.md
│   └── troubleshooting.md
│
└── scripts/
    ├── build.ps1
    └── run.ps1
```

Adapt the structure when necessary, but maintain the same separation of responsibilities.

---

# 7. OPENGL RENDERING REQUIREMENTS

OpenGL must be the actual renderer.

Implement:

## Window/context

- GLFW
- OpenGL 3.3+ Core Profile
- VSync configuration
- resizable window
- framebuffer resize handling

## Rendering

Implement:

- VAO
- VBO
- EBO/IBO where appropriate
- indexed rendering
- vertex attributes
- shader programs
- shader compilation error reporting
- shader linking error reporting
- uniform handling

## Depth

Enable:

```cpp
glEnable(GL_DEPTH_TEST);
```

Handle:

- depth testing
- depth clearing
- depth precision appropriately

## Face culling

Support:

- back-face culling
- configurable culling mode

## Blending

Support transparency for UI/selected overlays where necessary.

---

# 8. 3D OBJECT SYSTEM

Implement a reusable SceneObject class.

Each object must contain:

```text
ID
Name
PrimitiveType
Transform
Mesh
Material
BoundingVolume
Selected
Visible
Locked
```

Transform:

```text
position
rotation
scale
```

Use GLM matrices.

The model matrix should be calculated correctly from:

```text
Translation
Rotation
Scale
```

Support rotation around X/Y/Z.

Use quaternion internally if beneficial to prevent gimbal issues, while still exposing Euler angles for UI/debugging.

---

# 9. REQUIRED 3D PRIMITIVES

Implement at minimum:

- Cube
- Sphere
- Cylinder
- Cone
- Plane
- Torus

Each primitive must be generated procedurally or provided through a clear reusable mesh system.

The user must be able to create multiple instances of each.

---

# 10. OPENGL CAMERA SYSTEM

Implement a proper 3D camera.

Support:

- perspective projection
- configurable field of view
- near/far clipping planes
- camera position
- camera orientation
- view matrix
- projection matrix

Use:

```text
MVP = Projection × View × Model
```

Keep camera code independent from hand gesture processing.

Implement optional gesture-controlled camera movement, but make it possible to disable it.

---

# 11. LIGHTING

Implement real OpenGL lighting.

At minimum:

- directional light
- point light
- ambient component
- diffuse component
- specular component

Use a Phong or Blinn-Phong style lighting model.

Support configurable parameters such as:

```text
ambient
diffuse
specular
shininess
light position
light direction
```

Make lighting visibly demonstrate the effect of 3D transformations and materials.

---

# 12. MATERIAL SYSTEM

Create a reusable material system.

Support at minimum:

- matte
- glossy
- metallic
- transparent/glass-like
- emissive

Material properties should include suitable shader parameters.

Allow changing material through UI and/or gesture-controlled commands.

---

# 13. OBJECT SELECTION

Implement reliable object selection.

Preferred approach:

- ray casting from a virtual screen-space cursor
- ray/primitive or ray/bounding-box intersection

The hand should control the cursor.

Calculate:

```text
normalized device coordinates
        ↓
inverse projection/view
        ↓
world-space ray
        ↓
ray-object intersection
```

Do not fake selection using only screen-space overlap.

Implement visual feedback:

- outline
- highlight
- bounding box
- transform indicator

Selected objects must be clearly recognizable.

---

# 14. HAND TRACKING

Use MediaPipe for hand landmark detection.

Track:

- one hand
- two hands

Use the 21 landmarks.

Create a clear abstraction for:

```text
Hand
├── landmarks
├── wrist
├── palm center
├── index tip
├── thumb tip
├── middle tip
├── ring tip
├── pinky tip
├── handedness
├── confidence
└── derived metrics
```

Derived metrics should include:

- finger extension state
- pinch distance
- palm center
- palm orientation
- hand size estimate
- finger angles
- relative landmark distances

---

# 15. GESTURE RECOGNITION

Do NOT implement gestures using one-frame hardcoded checks only.

Implement smoothing and a gesture state machine.

Required gestures:

## Point

Index finger extended.

Purpose:

- cursor control
- selection

## Pinch

Thumb and index finger close together.

Purpose:

- grab
- manipulate
- confirm

## Open Palm

Most fingers extended.

Purpose:

- neutral state
- create/menu gesture
- release state depending on context

## Fist

Fingers folded.

Purpose:

- delete
- cancel
- command gesture

## Two-hand pinch

Both hands pinching.

Purpose:

- two-hand scale
- advanced transformations

## Additional gesture

Implement one additional robust gesture such as:

- two-finger gesture
- thumbs-up
- three-finger gesture

Use it for duplication, reset, or mode switching.

---

# 16. GESTURE STABILITY

This is critical.

Avoid accidental actions.

Implement:

- moving-average smoothing
- exponential smoothing or equivalent
- configurable thresholds
- gesture confidence
- minimum hold duration
- hysteresis
- cooldown periods
- state transitions

For example:

```text
PINCH_START
PINCH_HOLD
PINCH_MOVE
PINCH_RELEASE
```

Do not repeatedly trigger creation/deletion every frame.

A destructive command must require deliberate recognition.

---

# 17. TOUCHLESS 3D CURSOR

Implement a virtual 3D interaction cursor.

The point/index finger controls the cursor.

Display it visually using OpenGL.

The cursor should:

- move in screen space
- optionally estimate depth
- show its relationship to selected objects
- snap to objects/grid where appropriate

Use this cursor for selection and manipulation.

---

# 18. OBJECT TRANSLATION

Required behavior:

```text
Point at object
        ↓
Pinch
        ↓
Grab object
        ↓
Move hand
        ↓
Object follows cursor
        ↓
Release pinch
        ↓
Object remains in new position
```

Implement stable mapping between hand movement and world-space movement.

Avoid excessive jitter.

Provide sensitivity configuration.

Support movement constrained along:

- X
- Y
- Z

where possible.

Allow axis-lock controls.

---

# 19. OBJECT ROTATION

Implement intuitive rotation.

Possible gestures:

- rotating the hand
- one-hand controlled rotation
- two-hand relative orientation

Derive rotation from actual landmark geometry.

Do not use arbitrary random rotation values.

Support:

- X rotation
- Y rotation
- Z rotation
- free rotation

Apply smoothing.

Display rotation feedback where useful.

---

# 20. TWO-HAND SCALING

Use both hands.

Calculate distance between hand centers.

Example:

```text
hands move apart
        ↓
scale increases

hands move closer
        ↓
scale decreases
```

Use a reference distance:

```text
scaleFactor = currentDistance / initialDistance
```

Apply sensible clamping.

Avoid scale explosion.

---

# 21. TWO-HAND ROTATION

Implement an advanced gesture:

```text
Left hand + right hand
        ↓
vector between hands
        ↓
compare current vector with initial vector
        ↓
calculate angular change
        ↓
rotate object
```

This should provide a convincing two-handed object manipulation mode.

---

# 22. OBJECT CREATION

Implement gesture-driven creation.

The user should be able to create:

- cube
- sphere
- cylinder
- cone
- torus

Use an interaction mode/menu.

Example concept:

```text
Open palm
   ↓
Creation mode
   ↓
Point to virtual menu item
   ↓
Pinch
   ↓
Create primitive
   ↓
Object appears at cursor
```

Do not require keyboard input for normal creation.

Keyboard controls can exist as fallback/debug functions.

---

# 23. DUPLICATION

Implement object duplication.

Gesture:

- dedicated two/three-finger gesture OR UI gesture

On duplication:

- create a new object
- copy transform/material/mesh data
- assign a new unique ID
- offset slightly to prevent z-fighting/overlap confusion
- select the duplicate

---

# 24. OBJECT DELETION

Implement deletion using a deliberate gesture.

Recommended:

```text
select object
      ↓
fist / delete gesture
      ↓
hold confirmation briefly
      ↓
delete
```

Do not immediately delete on noisy gesture detection.

Provide undo support.

---

# 25. UNDO / REDO

Implement a command/history system.

Support:

- create
- delete
- move
- rotate
- scale
- material change
- duplicate

At minimum:

```text
Ctrl+Z
Ctrl+Y
```

and preferably gesture-accessible undo.

Keep enough state to restore operations correctly.

---

# 26. GRID AND AXIS SYSTEM

Render a professional 3D workspace.

Add:

- floor/grid
- X axis
- Y axis
- Z axis
- configurable grid spacing
- origin marker

Use subtle rendering so the scene remains visually clear.

---

# 27. TRANSFORM GIZMOS

Create an OpenGL transform gizmo for selected objects.

Support visual:

- X axis
- Y axis
- Z axis
- rotation rings
- scale handles if practical

The primary control remains gestures.

The gizmo is primarily for visualization and optional mouse debugging.

---

# 28. VISUAL FEEDBACK

The user should always know:

- current gesture
- selected object
- current operation
- whether pinch is active
- whether object is being moved
- whether two-hand mode is active

Display a lightweight HUD.

Example:

```text
GESTURE: PINCH
MODE: TRANSLATE
OBJECT: Cube_03

TRACKING
Left Hand: OK
Right Hand: OK
FPS: 58
```

Allow debug UI to be toggled.

---

# 29. LIVE HAND FEEDBACK

Provide a webcam/hand-tracking overlay option.

Possible modes:

1. Pure 3D scene
2. Scene + small webcam feed
3. Scene + landmark overlay
4. Full debug view

Do not allow the webcam overlay to interfere with the main 3D viewport.

---

# 30. SCENE MANAGEMENT

Support:

- multiple objects
- unique IDs
- object names
- visibility
- selection
- locking
- deletion
- duplication

Implement a SceneManager.

Provide a scene hierarchy/list in the UI.

Example:

```text
SCENE
├── Cube_01
├── Sphere_01
├── Cylinder_01
├── Torus_01
└── Cone_01
```

---

# 31. SAVE / LOAD

Implement scene serialization.

Save:

- object IDs
- names
- type
- position
- rotation
- scale
- materials
- visibility
- other relevant state

Use JSON or another human-readable format.

Example:

```text
scenes/
    demo_scene.json
    test_scene.json
```

Provide:

- Save
- Load
- Save As
- New Scene

---

# 32. UI

Build a clean, modern UI.

Use an appropriate immediate-mode UI framework such as Dear ImGui if needed.

UI should provide:

- scene list
- selected object information
- material controls
- rendering settings
- gesture settings
- tracking status
- camera settings
- lighting controls
- save/load controls
- help

Do NOT make the UI dominate the screen.

The 3D viewport should remain the focus.

---

# 33. DEBUGGING TOOLS

This project must be debuggable.

Implement debug modes for:

## Hand landmarks

Show all 21 landmarks and connections.

## Gesture metrics

Display values such as:

```text
pinchDistance
palmAngle
handDistance
handConfidence
fingerStates
```

## Interaction

Show:

- ray
- cursor
- intersection point
- selected object bounds

## Rendering

Show:

- FPS
- frame time
- draw call count if practical
- active object count

These debug views should be toggleable.

---

# 34. LOGGING

Implement structured logging.

Levels:

- DEBUG
- INFO
- WARNING
- ERROR
- CRITICAL

Log important events:

- startup
- camera initialization
- MediaPipe initialization
- shader errors
- object creation
- object deletion
- gesture state transitions
- scene loading
- scene saving
- unexpected failures

Avoid flooding logs every frame.

---

# 35. ERROR HANDLING

Handle:

- webcam unavailable
- webcam permission failure
- MediaPipe initialization failure
- OpenGL initialization failure
- shader compilation errors
- invalid scene file
- missing assets
- invalid gesture data
- lost hand tracking
- malformed configuration

The application must fail gracefully.

Never silently continue after critical initialization failure.

---

# 36. CONFIGURATION

Create a configuration file.

Allow configuration for:

```text
camera index
camera width
camera height
FPS
gesture thresholds
gesture smoothing
pinch threshold
tracking confidence
render resolution
FOV
near/far planes
light parameters
grid settings
UI settings
debug settings
```

Do not hardcode everything.

---

# 37. PERFORMANCE

Target:

- stable real-time interaction
- preferably 30–60 FPS
- low gesture latency

Optimize:

- unnecessary allocations
- texture uploads
- repeated mesh generation
- OpenCV image conversion
- logging
- shader recompilation
- scene traversal

Do not prematurely optimize complex code at the cost of correctness.

Profile obvious bottlenecks.

---

# 38. THREADING

Consider separating:

```text
Webcam capture
       ↓
Hand tracking
       ↓
Rendering
```

into appropriate threads if MediaPipe processing becomes a bottleneck.

However:

- do not introduce race conditions
- keep scene modification on the correct thread
- protect shared data
- prefer a clean producer/consumer approach

A simpler single-threaded version is acceptable if it reliably maintains target FPS.

---

# 39. CAMERA-TO-WORLD MAPPING

This is a critical technical area.

Do not simply map:

```text
hand_x → object_x
hand_y → object_y
```

without considering the 3D camera.

Implement a proper mapping based on:

- viewport
- projection
- camera
- depth/reference plane
- world coordinates

Use unprojection where appropriate.

Document the mathematics in code comments and documentation.

---

# 40. DEPTH / Z CONTROL

Because a normal webcam does not directly provide precise depth, do NOT pretend it has perfect physical depth.

Implement a practical interaction model.

Possible approaches:

- hand-relative depth estimation
- MediaPipe landmark depth
- palm-size heuristic
- two-hand distance
- selectable interaction planes
- depth sensitivity parameter

Use these intelligently.

Clearly document the limitations.

Do not claim sensor-level depth accuracy.

---

# 41. INTERACTION MODES

Create explicit interaction modes:

```text
SELECT
TRANSLATE
ROTATE
SCALE
CREATE
DELETE
CAMERA
```

Modes may be selected by gestures or UI.

Use state transitions to prevent gesture ambiguity.

For example:

```text
Point
 ↓
SELECT

Point + Pinch
 ↓
TRANSLATE

Two Hand Pinch
 ↓
SCALE/ROTATE
```

Make the behavior configurable.

---

# 42. GESTURE STATE MACHINE

Implement something similar to:

```text
NO_HAND
   ↓
TRACKING
   ↓
POINTING
   ↓
PINCH_START
   ↓
PINCH_HOLD
   ↓
MANIPULATING
   ↓
PINCH_RELEASE
   ↓
TRACKING
```

For destructive actions:

```text
GESTURE_DETECTED
       ↓
CONFIRM_TIMER
       ↓
ACTION
```

This is required to reduce accidental actions.

---

# 43. MULTI-HAND STATE

Track each hand separately.

Example:

```text
LeftHandState
RightHandState
```

Then derive:

```text
SingleHandMode
TwoHandMode
```

Do not assume hand order is fixed if MediaPipe provides handedness information.

Handle temporary loss of one hand gracefully.

---

# 44. TEMPORAL SMOOTHING

Implement configurable smoothing.

Potential technique:

```text
smoothed = alpha * current + (1-alpha) * previous
```

Apply appropriate smoothing separately to:

- cursor position
- pinch distance
- hand rotation
- two-hand distance
- gesture probabilities

Avoid excessive smoothing that introduces obvious lag.

---

# 45. OBJECT MANIPULATION QUALITY

This project will be judged heavily by interaction quality.

The object must not:

- jitter excessively
- jump when grabbed
- suddenly change scale
- rotate uncontrollably
- move with significant lag

When grabbing an object, preserve the initial offset between:

```text
cursor position
object position
```

This prevents snapping.

---

# 46. RENDERING MODES

Implement:

- solid
- wireframe
- solid + wireframe selected outline

Allow toggling from UI.

---

# 47. OPTIONAL SHADOW SYSTEM

If performance permits, implement shadow mapping.

At minimum:

- directional light shadow map
- depth framebuffer
- shadow comparison

Do not add shadows if they destabilize the core application.

The core gesture/modeling functionality has priority.

---

# 48. ADVANCED VISUAL FEATURES

After the core system works, consider:

- object outlines
- hover highlighting
- glow for selected objects
- translucent selection planes
- grid snapping
- axis snapping
- object snapping
- smooth animation of UI
- subtle cursor trails

Do not overdo visual effects.

The application should look professional, not like a game.

---

# 49. GRID SNAPPING

Implement optional snapping:

```text
position → nearest grid increment
```

Make grid spacing configurable.

Also consider angle snapping:

```text
rotation → nearest 15°
```

and scale snapping where practical.

---

# 50. OBJECT ALIGNMENT

Optionally implement simple alignment tools:

- center
- align X
- align Y
- align Z
- distribute

These may be UI/keyboard tools if gesture implementation becomes ambiguous.

---

# 51. FALLBACK INPUT

Although the project is gesture-first, provide keyboard/mouse fallback for development.

Keyboard examples:

```text
W = move
E = rotate
R = scale
Delete = delete
Ctrl+Z = undo
Ctrl+Y = redo
```

Mouse can optionally control camera and selection during testing.

The final demonstration should emphasize gesture interaction.

---

# 52. ACCESSIBILITY / CONTROL

Provide controls for:

- gesture sensitivity
- smoothing
- camera inversion if needed
- left/right hand preference
- webcam selection
- debug mode

These should be configurable.

---

# 53. TESTING

Create unit tests for:

- gesture metric calculations
- pinch detection
- finger-state detection
- smoothing
- transformation matrices
- ray construction
- ray/AABB intersection
- scene serialization
- undo/redo

Create integration tests for:

- camera startup
- scene creation
- object transformations
- scene save/load

If real webcam automation is impractical, provide replay/test data abstractions.

---

# 54. GESTURE TEST MODE

Implement a mode where gesture input can come from recorded/simulated landmark coordinates.

This is important because it allows debugging without continuously using the webcam.

Example:

```text
tests/data/
    pinch.json
    pointing.json
    two_hand_scale.json
```

The gesture engine should be testable independently of the camera.

---

# 55. HAND TRACKING FAILURE BEHAVIOR

When the hand disappears:

- do not delete objects
- do not continue manipulating indefinitely
- release temporary grab state after timeout
- reset transient gesture state
- display tracking lost status

This is essential for real usability.

---

# 56. SAFETY AGAINST ACCIDENTAL DESTRUCTION

Deletion should require:

- selected object
- recognized delete gesture
- confidence
- hold duration

Do not allow a noisy frame to destroy scene objects.

---

# 57. PERFORMANCE TELEMETRY

Display optionally:

```text
FPS
CPU time
GPU-related metrics where available
hand tracking latency
render latency
object count
```

Do not make unsupported claims about GPU metrics.

---

# 58. CODE QUALITY

Use:

- RAII
- smart pointers where appropriate
- const correctness
- enum classes
- clear interfaces
- small focused classes
- meaningful naming
- minimal global state
- no magic numbers
- configuration instead of hardcoding

Avoid giant classes such as:

```text
GodObject
```

where webcam, rendering, gesture recognition, scene management, and UI all live together.

---

# 59. COMMENTS

Comments should explain:

- non-obvious mathematics
- coordinate transformations
- gesture thresholds
- depth estimation
- OpenGL details
- synchronization

Do not write comments that merely restate the code.

---

# 60. DOCUMENTATION

Create:

## README.md

Include:

- project description
- features
- architecture
- prerequisites
- installation
- build
- run
- controls
- gestures
- troubleshooting

## architecture.md

Explain:

```text
OpenCV
MediaPipe
Gesture Engine
Interaction System
Scene
OpenGL Renderer
```

## graphics_pipeline.md

Explain:

```text
Model
View
Projection
Vertex shader
Rasterization
Fragment shader
Depth testing
Lighting
Materials
```

## gestures.md

Document every gesture and thresholds.

## build.md

Provide exact Windows build instructions.

---

# 61. DEMONSTRATION MODE

Create a demo-ready startup configuration.

When launched:

- create a visually appealing scene
- show several 3D objects
- enable lighting
- enable grid
- display minimal HUD
- enable webcam tracking
- show gesture status

The application should immediately look polished without requiring setup every time.

---

# 62. DEMO SCENARIO

The final demo should be capable of this sequence:

1. Start application.
2. Webcam begins tracking hand.
3. Point at cube.
4. Select cube.
5. Pinch and move cube.
6. Release.
7. Rotate cube with hand.
8. Use two hands to scale cube.
9. Create a sphere using the gesture-controlled creation interface.
10. Move sphere.
11. Change its material.
12. Duplicate it.
13. Delete the duplicate.
14. Undo deletion.
15. Save the scene.
16. Load the scene.
17. Manipulate multiple objects.
18. Demonstrate lighting/material effects.

This sequence should be stable enough for a live presentation.

---

# 63. VISUAL DESIGN

Aim for:

- dark or neutral professional workspace
- clean 3D viewport
- subtle grid
- clear selected-object outline
- readable HUD
- minimal clutter
- high contrast
- smooth transitions

Do not use excessive animations or distracting elements.

---

# 64. REQUIRED DEVELOPMENT PHASES

Implement in phases.

## Phase 1 — Foundation

Build:

- CMake
- GLFW
- GLAD
- OpenGL initialization
- shaders
- triangle
- basic camera

Do not proceed until rendering works.

## Phase 2 — 3D Engine

Add:

- cube
- sphere
- cylinder
- transformations
- camera
- depth testing
- lighting
- materials

## Phase 3 — OpenCV

Add:

- webcam capture
- frame conversion
- configurable camera settings

## Phase 4 — MediaPipe

Add:

- hand landmarks
- hand confidence
- handedness
- debug landmark visualization

## Phase 5 — Gesture Engine

Implement:

- point
- pinch
- open palm
- fist
- two-hand recognition
- temporal smoothing
- state machine

## Phase 6 — Interaction

Implement:

- cursor
- selection
- translation
- rotation
- scaling

## Phase 7 — Modeling

Implement:

- object creation
- duplication
- deletion
- materials
- scene management

## Phase 8 — Professional Features

Implement:

- UI
- undo/redo
- save/load
- grid
- snapping
- transform gizmos
- camera gestures
- debug tools

## Phase 9 — Polish

Improve:

- gesture reliability
- performance
- visuals
- error handling
- documentation
- demo workflow

---

# 65. DEVELOPMENT RULE

Do not generate the entire project as one huge untested code dump.

Build incrementally.

After every major subsystem:

1. compile
2. test
3. fix errors
4. keep the project runnable

Never knowingly leave the repository in a broken state.

---

# 66. BUILD SYSTEM

Use CMake.

The project must support something close to:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Provide scripts for convenience.

Document dependency installation.

---

# 67. THIRD-PARTY DEPENDENCIES

Minimize dependencies.

Preferred:

- OpenGL
- GLFW
- GLAD
- GLM
- OpenCV
- MediaPipe
- ImGui if required

Record exact versions in documentation.

Use reproducible dependency setup where reasonably possible.

---

# 68. SEPARATION OF EXPERIMENTAL FEATURES

Keep experimental features behind configuration flags.

Examples:

```text
ENABLE_SHADOWS
ENABLE_HAND_CAMERA_OVERLAY
ENABLE_GESTURE_CAMERA
ENABLE_TWO_HAND_ROTATION
ENABLE_GRID_SNAP
ENABLE_DEBUG_OVERLAY
```

This allows the stable demo build to disable experimental functionality.

---

# 69. CRITICAL REQUIREMENT: NO FAKE IMPLEMENTATION

Do not implement fake versions of required functionality.

For example:

BAD:

```text
gesture detected
→ rotate object by a fixed amount
```

GOOD:

```text
gesture detected
→ calculate actual hand orientation change
→ convert orientation delta
→ apply transformation
```

BAD:

```text
hand X coordinate copied directly to object X
```

GOOD:

```text
hand/cursor coordinates
→ viewport conversion
→ camera-space/world-space mapping
→ appropriate object transformation
```

BAD:

```text
OpenGL window containing prerecorded animation
```

GOOD:

```text
real-time OpenGL scene updated from real-time gesture input
```

---

# 70. COMPUTER GRAPHICS INTEGRITY

The final project must clearly be a Computer Graphics project.

The implementation must visibly and technically demonstrate:

```text
Geometric modeling
+
Transformations
+
Projection
+
Camera
+
Lighting
+
Materials
+
Depth
+
Rendering
+
Interactive manipulation
```

OpenGL must perform the graphics operations.

---

# 71. ACADEMIC QUALITY

Code should be suitable for explaining during a viva.

For each major feature, make it possible to explain:

- what it does
- why it exists
- mathematical basis
- algorithm
- implementation
- limitations

Do not hide core logic behind opaque helper libraries.

Third-party libraries can handle infrastructure, but the project's own implementation should contain the core:

- transformation handling
- interaction mapping
- gesture interpretation
- selection/raycasting
- scene management

---

# 72. FINAL DELIVERABLE

At the end, provide:

1. Fully compilable source code
2. CMake build
3. Shaders
4. Configuration
5. Documentation
6. Tests
7. Demo scene
8. Example saved scene
9. Build scripts
10. Troubleshooting guide

Also provide a final summary containing:

```text
Implemented Features
Known Limitations
Build Instructions
Run Instructions
Gesture Controls
Keyboard Fallback
Project Architecture
Future Improvements
```

---

# 73. KNOWN LIMITATIONS

Be technically honest.

Explicitly document limitations such as:

- monocular webcam depth uncertainty
- lighting-dependent hand tracking
- occlusion
- gesture ambiguity
- tracking loss
- performance variations by hardware

Do not claim full professional CAD precision.

---

# 74. FINAL QUALITY STANDARD

The finished project should satisfy this standard:

A person unfamiliar with the implementation should be able to launch it, see a polished OpenGL 3D environment, show their hand to the webcam, point at an object, select it, pinch it, move it, rotate it, scale it with two hands, create another object, alter its material, duplicate it, delete it, undo the operation, save the scene, and continue interacting naturally.

The result must look and behave like a coherent application, not a collection of disconnected demos.

---

# 75. CODING WORKFLOW FOR YOU

Work directly inside the repository.

Before implementing anything substantial:

1. Inspect the current repository.
2. Identify existing files/code.
3. Preserve useful existing work.
4. Create a clean architecture if the repository is empty.
5. Build the smallest working version first.
6. Compile frequently.
7. Run tests frequently.
8. Fix compiler warnings that indicate real problems.
9. Keep changes modular.

When a requirement cannot be implemented exactly because a library/API differs from expectations, determine the correct API from the installed version rather than inventing one.

Do not fabricate library functions.

---

# 76. EXPECTED FINAL RESPONSE FROM CODEX

When the implementation is complete, report:

### Build Status
- compiler
- build configuration
- build result

### Runtime Status
- OpenGL version
- webcam status
- hand tracking status

### Implemented Features
List the actual working features.

### Tests
List tests created and their results.

### Known Issues
List anything that remains unresolved.

### How to Run
Give exact commands.

### Demo Sequence
Give the recommended live demonstration sequence.

Do not claim a feature is complete unless it has actually been implemented and tested.

---

# FINAL INSTRUCTION

Treat this as a serious engineering project, not a toy example.

The most important success criterion is:

**A stable, visually polished, genuinely interactive OpenGL 3D modeling environment controlled by real-time hand gestures using MediaPipe and OpenCV.**

Make the system modular, testable, debuggable, and demonstrable.

Build the core first, then add advanced features without destabilizing the system.