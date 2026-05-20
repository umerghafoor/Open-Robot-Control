# OpenRobotControl

A general-purpose, open-source Qt6 desktop interface for robot control, telemetry, and simulation — with optional ROS2 integration.

OpenRobotControl provides a dockable widget workspace, a Digital Twin engine, and a clean message bridge to ROS2 so you can wire up new robots, sensors, and control surfaces without rebuilding UI plumbing every time.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt6](https://img.shields.io/badge/Qt-6-41CD52)](https://www.qt.io/)
[![ROS2 optional](https://img.shields.io/badge/ROS2-optional-22314E)](https://docs.ros.org/)

## TL;DR

- **Stack:** C++17 + Qt6 (`Core`, `Gui`, `Widgets`, `Network`), optional Qt Multimedia / OpenGL, optional ROS2.
- **Entry flow:** `main.cpp` → `Application` → `ROS2Interface` → `DigitalTwin` → `MainWindow`.
- **UI model:** dockable widget workspace managed by `WidgetManager` — rearrange, hide, and persist layouts at runtime.
- **ROS2 is optional:** if ROS2 is not sourced/found, the app builds and runs in stub/standalone mode.

## Why this project

Most robot UIs are tightly coupled to a specific platform, fleet, or stack. OpenRobotControl is the opposite: a hackable, dock-based shell you can extend for any robot — wheeled, tracked, arm, drone, or simulated — with or without ROS2. Contributions, forks, and platform-specific layouts are welcome.

## Core Capabilities

- Modular dockable widgets (add/remove/rearrange at runtime, layouts persist).
- Robot command publishing (`/cmd_vel`, `/robot_command`).
- Real-time telemetry subscriptions (camera, IMU, status, coordinates).
- Digital Twin modes:
  - `Synchronized` (mirrors ROS2 updates)
  - `Simulated` (internal physics simulation)
  - `Offline` (standalone)
- Structured logging to `OpenRobotControl.log`.
- Optional 3D model viewport — drop in your own `robot.obj` to visualize your platform.

## Widget Set (Current)

Registered by default in `WidgetManager`:

- `Video Stream`
- `Motion Control`
- `Command & Control`
- `Sensor Data`
- `Coordinates`
- `Digital Twin` (type exists; UI creation is currently disabled in `MainWindow`)

## ROS2 Contract (Code-Accurate)

### Published

- `/cmd_vel` (`geometry_msgs/msg/Twist`)
- `/robot_command` (`std_msgs/msg/String`)

### Subscribed

- `camera/raw` (`sensor_msgs/msg/Image`)
- `/imu/data` (`sensor_msgs/msg/Imu`)
- `/robot_status` (`std_msgs/msg/String`)
- `/coordinates` (`geometry_msgs/msg/PointStamped`)
- `image/coordinates` (`std_msgs/msg/String`, JSON payload)

### Notes

- Image encoding: incoming `bgr8` frames are converted to RGB before Qt signal emission.
- ROS2 spinning runs on a dedicated `QThread` with a `QTimer` at 10 ms (`spin_some` loop).
- Camera topic can be switched dynamically via `ROS2Interface::switchCameraTopic(...)`.

## Build

```bash
./build.sh clean debug
# or
./build.sh clean release
```

What `build.sh` does:

- Detects ROS2 via `ROS_DISTRO`.
- Sets `-DUSE_ROS2=OFF` automatically when ROS2 is not sourced.
- Configures CMake and builds `build/OpenRobotControl`.

### Manual CMake (optional)

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_ROS2=ON
make -j"$(nproc)"
```

## Run

```bash
./run.sh
```

- Auto-builds if executable is missing.
- Runs in standalone mode when ROS2 is unavailable.
- Prints last log entries on exit.

## Runtime UX Summary

- Main window starts with a default dock layout (video/coordinates left, controls right, sensor bottom).
- Menu groups: `File`, `Widgets`, `ROS2`, `Simulation`, `Help`.
- ROS2 connection is toggled from the UI (`Connect` action).
- Simulation is toggled from the UI (`Start Simulation` action).

## Project Map

- `src/main.cpp` — Qt app bootstrap + logger init.
- `src/core/Application.*` — lifecycle orchestration and dependency wiring.
- `src/core/WidgetManager.*` — widget registration/factory/active instances.
- `src/ros2/ROS2Interface.*` — ROS2 pub/sub + Qt bridge.
- `src/twin/DigitalTwin.*` — twin mode/state and simulation control.
- `src/ui/MainWindow.*` — menus, docking, default layout.
- `src/ui/widgets/*` — per-widget UI and behavior.
- `src/utils/Logger.*` — centralized logging.

## Extending OpenRobotControl

### Add a new widget

1. Create a widget class deriving from `BaseWidget`.
2. Register an enum/name and factory case in `WidgetManager`.
3. Add source/header to `CMakeLists.txt`.
4. Expose a menu/toolbar action in `MainWindow` if user-visible.

### Add a ROS2 topic

1. Add publisher/subscriber in `ROS2Interface`.
2. Bridge data through Qt signals.
3. Consume in widgets and/or `DigitalTwin`.

### Use your own robot model

Drop a `robot.obj` (optionally with `robot.mtl` and `robot.png`) next to the built executable or in the project root. The 3D model widget picks it up automatically. If no `robot.obj` is found, the bundled `box.obj` sample is shown as a placeholder.

## Requirements

- Linux (Ubuntu recommended; other platforms welcome — contributions invited).
- CMake >= 3.16
- C++17 toolchain
- Qt6 base development packages
- Optional: ROS2 (Humble or newer) for live robot integration

Install build dependencies on Ubuntu:

```bash
./install_dependency.sh
```

## Troubleshooting

- Build without ROS2 intentionally: leave ROS2 unsourced, then run `./build.sh`.
- If camera appears wrong-color, verify source encoding (`bgr8` vs `rgb8`).
- Check logs in `OpenRobotControl.log` for startup/init/connectivity issues.

## Contributing

OpenRobotControl is community-driven. Bug reports, feature ideas, widgets for new robots, and docs improvements are all welcome:

1. Fork the repo and create a feature branch.
2. Keep changes surgical and preserve the dock-based architecture unless proposing a UI redesign.
3. Open a pull request describing the change and any robot/platform you tested against.

## License

[MIT](LICENSE). Use it, fork it, ship it.
