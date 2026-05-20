# Documentation Index

Welcome to the **OpenRobotControl** documentation.

OpenRobotControl is a general-purpose, open-source Qt6 desktop interface for robot control, telemetry, and simulation, with optional ROS2 integration.

## Quick Navigation

### For first-time users

1. **[../README.md](../README.md)** — overview and feature list
2. **[QUICKSTART.md](QUICKSTART.md)** — get up and running in a few minutes
3. **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** — what's included

### For developers

1. **[ARCHITECTURE.md](ARCHITECTURE.md)** — system architecture and design
2. **[FILE_STRUCTURE.md](FILE_STRUCTURE.md)** — project organization
3. **[DIAGRAMS.md](DIAGRAMS.md)** — visual diagrams
4. **[ROS2_NODES.md](ROS2_NODES.md)** — ROS2 topic reference
5. **[ROS2-Communication.md](ROS2-Communication.md)** — multi-device DDS setup

### Build & configuration

1. **[../build.sh](../build.sh)** — automated build script
2. **[../CMakeLists.txt](../CMakeLists.txt)** — CMake configuration
3. **[../package.xml](../package.xml)** — ROS2 package manifest

## Document Summaries

### README.md

Main project documentation. Audience: everyone. Read this first.

### QUICKSTART.md

Step-by-step build, run, and first-time setup guide. Audience: new users.

### ARCHITECTURE.md

System architecture, module descriptions, design patterns, threading model, data flow, and extensibility points. Audience: developers.

### FILE_STRUCTURE.md

Directory layout, file descriptions, and module dependencies. Audience: developers navigating the codebase.

### DIAGRAMS.md

Architecture, data-flow, threading, and lifecycle diagrams. Audience: developers and technical users.

### PROJECT_SUMMARY.md

Complete project overview, features, and roadmap. Audience: everyone.

### ROS2_NODES.md

ROS2 topic contract and `ros2` CLI cheatsheet. Audience: anyone wiring OpenRobotControl to a robot.

### ROS2-Communication.md

Setting up DDS / CycloneDDS communication between multiple machines (e.g. a Raspberry Pi and a Dockerized host). Audience: integrators.

## Quick Reference

### Build commands

```bash
# Clean build
./build.sh clean release

# Debug build
./build.sh clean debug

# Run
./run.sh
```

### File locations

```text
Source Code:        src/
Build Output:       build/
Executable:         build/OpenRobotControl
Log File:           OpenRobotControl.log
Documentation:      docs/*.md
```

### Key classes

```text
Core:        Application, WidgetManager
ROS2:        ROS2Interface
Twin:        DigitalTwin, TwinState, TwinSimulator
UI:          MainWindow, BaseWidget
Widgets:     VideoStream, CommandControl, MotionControl,
             SensorData, Coordinates, RobotModel, IMU3D,
             Detection*, TwinVisualization
Utils:       Logger
```

### ROS2 topics

```text
Subscribe:   camera/raw, /imu/data, /robot_status,
             /coordinates, image/coordinates
Publish:     /cmd_vel, /robot_command
```

## Contributing to Documentation

When contributing, update:

- `README.md` for new features
- `docs/ARCHITECTURE.md` for design changes
- `docs/QUICKSTART.md` for setup changes
- `docs/FILE_STRUCTURE.md` for new files
- `docs/DIAGRAMS.md` for architecture changes

## Getting Help

1. **Check documentation** — start here.
2. **Review the code** — widgets under `src/ui/widgets/` are good examples.
3. **Check logs** — `OpenRobotControl.log` in the project root.
4. **File a GitHub issue** — bug reports and feature requests welcome.

## License

OpenRobotControl is released under the [MIT License](../LICENSE).
