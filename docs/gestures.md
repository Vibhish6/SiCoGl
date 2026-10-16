# Gesture model

Landmarks are expected in normalized image coordinates. The mirrored index-tip position drives the normalized viewport cursor. Pinch is the thumb/index distance divided by palm size; this makes the threshold less sensitive to distance from the webcam. Point, open palm, fist, and three-finger states are derived from tip/PIP vertical relationships.

Gesture processing is temporal: cursor coordinates use exponential smoothing, pinch uses separate start/release thresholds, and deletion requires a configurable fist hold plus a cooldown. Lost tracking resets transient pinch state so a disappearing hand cannot continue moving or deleting an object.

Two-hand pinch exposes the vector, distance, and angle between palm centers. The interaction system uses the ratio of current to initial distance for clamped scaling and the wrap-safe angle delta for twist rotation. A normal webcam does not provide metric depth, so translation uses a documented z=0 interaction plane and camera-ray unprojection rather than pretending to recover exact physical depth. Selection transforms the ray into each object's local space before testing its local AABB, so rotated and non-uniformly scaled objects remain selectable.

Keyboard/mouse fallback is available for development: left-mouse drag selects/translates, `R` selects rotation mode, `G` selects scale mode, `T` selects translation mode, and `X/Y/Z` constrain translation. `P` arms gesture creation; the next pinch creates a cube at the cursor. A three-finger gesture duplicates the selected object.
