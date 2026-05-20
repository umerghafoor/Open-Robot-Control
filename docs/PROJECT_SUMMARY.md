# Project Summary

## OpenRobotControl

A general-purpose, open-source Qt6 desktop interface for robot control, telemetry, and simulation, with optional ROS2 integration.

OpenRobotControl is licensed under [MIT](../LICENSE) and welcomes contributions: new widgets, support for additional robot platforms, documentation improvements, and bug reports.

## What's Included

1. **Modular architecture**
   - Clean separation of ROS2, Digital Twin, and UI layers.
   - Plugin-ready widget system.

2. **ROS2 integration (optional)**
   - Thread-safe ROS2 interface.
   - Publishers and subscribers wired to Qt signals.
   - Non-blocking communication.

3. **Digital Twin module**
   - Three operating modes (`Synchronized` / `Simulated` / `Offline`).
   - Internal physics simulation.
   - State management.

4. **Modular UI system**
   - Dockable widget workspace.
   - Persistent layouts across sessions.
   - Easy to extend with platform-specific widgets.

5. **Documentation**
   - `README.md` — overview and reference.
   - `docs/QUICKSTART.md` — fast-start guide.
   - `docs/ARCHITECTURE.md` — technical deep dive.
   - `docs/FILE_STRUCTURE.md` — code organization.
   - `docs/DIAGRAMS.md` — visual diagrams.
   - `docs/ROS2_NODES.md` — topic reference.
   - `docs/ROS2-Communication.md` — multi-device setup.

## How to Use

### Quick Start

```bash
# 1. Clone the repo
git clone <your-fork-url> OpenRobotControl
cd OpenRobotControl

# 2. Build (auto-detects ROS2 via ROS_DISTRO)
./build.sh clean release

# 3. Run
./run.sh
```

### First Run Checklist

- [ ] (Optional) Source ROS2: `source /opt/ros/humble/setup.bash`
- [ ] Build: `./build.sh clean release`
- [ ] Run: `./run.sh` or `./build/OpenRobotControl`
- [ ] Add widgets via the `Widgets` menu
- [ ] Arrange the dock layout
- [ ] Connect to ROS2 or start the built-in simulation

## Features Implemented

### ROS2 integration (optional)

- Non-blocking threaded interface
- Velocity command publishing
- Image / IMU / status / coordinates subscriptions
- Qt signal/slot bridging

### Digital Twin

- Three operating modes
- State synchronization with ROS2
- Internal physics simulation
- Sensor modeling

### User Interface

- Dockable widget system with persistent layouts
- Menu and toolbar
- Status bar
- 3D model viewport (drop in your own `robot.obj`)

### Widgets

- Video Stream
- Motion Control
- Command & Control (with emergency stop)
- Sensor Data
- Coordinates
- Robot Model (3D)
- IMU 3D
- Detection panels
- Twin Visualization (type defined; UI disabled by default)

## Documentation Guide

### For users

- `README.md` — start here.
- `docs/QUICKSTART.md` — step-by-step setup.
- `docs/FILE_STRUCTURE.md` — what lives where.

### For developers

- `docs/ARCHITECTURE.md` — technical architecture details.
- `docs/DIAGRAMS.md` — visual diagrams.
- `docs/ROS2_NODES.md` — ROS2 topic contract.

## Future Enhancements

- [ ] Plugin system for third-party widgets
- [ ] Recording / playback of ROS2 sessions
- [ ] Map and path-planning widgets
- [ ] Multi-robot fleet view
- [ ] Cross-platform builds (Windows, macOS)
- [ ] Telemetry export

## Contributing

OpenRobotControl is community-driven. To contribute:

1. Fork the repo and create a feature branch.
2. Keep changes surgical and preserve the dock-based architecture unless proposing a UI redesign.
3. Open a pull request describing the change and any robot/platform you tested against.

## License

[MIT](../LICENSE). Use it, fork it, ship it.
