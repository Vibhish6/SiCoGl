"""Local webcam server for Gesture3D's Python MediaPipe fallback."""

import argparse
import json
import socket
import sys
import time
from pathlib import Path

import cv2
import mediapipe as mp


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", required=True)
    parser.add_argument("--camera", type=int, default=0)
    parser.add_argument("--width", type=int, default=640)
    parser.add_argument("--height", type=int, default=480)
    parser.add_argument("--fps", type=int, default=30)
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--ready-file", default="")
    parser.add_argument("--smoothing", type=float, default=0.35)
    parser.add_argument("--preview", action=argparse.BooleanOptionalAction, default=True)
    args = parser.parse_args()

    model = Path(args.model).resolve()
    if not model.is_file():
        print(f"Model not found: {model}", file=sys.stderr, flush=True)
        return 2

    vision = mp.tasks.vision
    options = vision.HandLandmarkerOptions(
        base_options=mp.tasks.BaseOptions(model_asset_path=str(model)),
        running_mode=vision.RunningMode.VIDEO,
        num_hands=2,
        min_hand_detection_confidence=0.40,
        min_hand_presence_confidence=0.40,
        min_tracking_confidence=0.40,
    )

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("127.0.0.1", args.port))
    server.listen(1)
    ready_file = Path(args.ready_file).resolve() if args.ready_file else None
    if ready_file:
        ready_file.write_text("ready\n", encoding="utf-8")
    print(f"Gesture3D MediaPipe bridge listening on 127.0.0.1:{args.port}", flush=True)
    connection, _ = server.accept()
    connection.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    camera = cv2.VideoCapture(args.camera, cv2.CAP_DSHOW)
    camera.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    camera.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)
    camera.set(cv2.CAP_PROP_FPS, args.fps)
    if not camera.isOpened():
        print(f"Unable to open webcam index {args.camera}", file=sys.stderr, flush=True)
        connection.close()
        server.close()
        return 3

    timestamp = 0
    previous_landmarks = {}
    smoothing = min(max(args.smoothing, 0.1), 0.8)
    frame_period = 1.0 / max(args.fps, 1)
    next_frame_time = time.perf_counter()
    try:
        with mp.tasks.vision.HandLandmarker.create_from_options(options) as landmarker:
            while True:
                ok, frame = camera.read()
                if not ok:
                    continue
                timestamp = max(timestamp + 1, int(time.monotonic() * 1000))
                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)
                result = landmarker.detect_for_video(image, timestamp)
                hands = []
                for landmarks, handedness in zip(result.hand_landmarks, result.handedness):
                    if len(landmarks) != 21 or not handedness:
                        continue
                    category = handedness[0]
                    label = category.display_name or category.category_name or ""
                    raw_landmarks = [[float(p.x), float(p.y), float(p.z)] for p in landmarks]
                    previous = previous_landmarks.get(label)
                    if previous is not None:
                        raw_landmarks = [[smoothing * current[j] + (1.0 - smoothing) * old[j] for j in range(3)] for current, old in zip(raw_landmarks, previous)]
                    previous_landmarks[label] = raw_landmarks
                    hands.append(
                        {
                            "left": label == "Left",
                            "confidence": float(category.score or 0.0),
                            "landmarks": raw_landmarks,
                        }
                    )
                if args.preview:
                    preview = cv2.flip(frame, 1)
                    preview_height, preview_width = preview.shape[:2]
                    for hand, landmarks in zip(hands, result.hand_landmarks):
                        points = [(int((1.0 - p[0]) * preview_width), int(p[1] * preview_height)) for p in hand["landmarks"]]
                        for a, b in ((0,1),(1,2),(2,3),(3,4),(0,5),(5,6),(6,7),(7,8),(5,9),(9,10),(10,11),(11,12),(9,13),(13,14),(14,15),(15,16),(13,17),(17,18),(18,19),(19,20),(0,17)):
                            cv2.line(preview, points[a], points[b], (80, 220, 255), 2, cv2.LINE_AA)
                        for point in points:
                            cv2.circle(preview, point, 5, (40, 255, 170), -1, cv2.LINE_AA)
                        label = ("Left" if hand["left"] else "Right") + "  " + f"{hand['confidence']:.0%}"
                        cv2.putText(preview, label, (max(8, points[0][0] - 45), max(24, points[0][1] - 18)), cv2.FONT_HERSHEY_SIMPLEX, 0.65, (255, 255, 255), 2, cv2.LINE_AA)
                    cv2.putText(preview, "Gesture3D camera  |  ESC/Q closes tracking", (16, 28), cv2.FONT_HERSHEY_SIMPLEX, 0.65, (255, 255, 255), 2, cv2.LINE_AA)
                    cv2.imshow("Gesture3D | Camera + Hand Tracking", preview)
                    if cv2.waitKey(1) & 0xFF in (27, ord("q")):
                        return 0
                message = json.dumps({"timestampMs": timestamp, "hands": hands}, separators=(",", ":")) + "\n"
                connection.sendall(message.encode("utf-8"))
                next_frame_time += frame_period
                delay = next_frame_time - time.perf_counter()
                if delay > 0:
                    time.sleep(delay)
                else:
                    next_frame_time = time.perf_counter()
    except (BrokenPipeError, ConnectionResetError):
        return 0
    finally:
        camera.release()
        if args.preview:
            cv2.destroyWindow("Gesture3D | Camera + Hand Tracking")
        connection.close()
        server.close()
        if ready_file:
            ready_file.unlink(missing_ok=True)


if __name__ == "__main__":
    raise SystemExit(main())
