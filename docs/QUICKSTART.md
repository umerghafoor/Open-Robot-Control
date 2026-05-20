# Quick Start Guide

## Getting Started with OpenRobotControl

### 1. Prerequisites Check

Before building, ensure you have:

```bash
# Check Qt6 installation
qmake6 --version

# Check ROS2 installation (optional)
echo $ROS_DISTRO

# Check C++ compiler
g++ --version
```

If any are missing:

```bash
# Install build tools and Qt6 on Ubuntu (or run ./install_dependency.sh)
sudo apt update
sudo apt install -y build-essential cmake git \
                    qt6-base-dev qt6-multimedia-dev qt6-tools-dev

# Install ROS2 (optional, only for live robot integration)
# Follow: https://docs.ros.org/en/humble/Installation.html
```

### 2. Build the Application

```bash
# Make build script executable
chmod +x build.sh

# Clean build (recommended for first time)
./build.sh clean release

# Or for debugging
./build.sh clean debug
```

The build script auto-detects ROS2 via `ROS_DISTRO`. If ROS2 is not sourced, the app builds in standalone (stub) mode.

### 3. Run the Application

```bash
./run.sh
# or directly
./build/OpenRobotControl
```

### 4. First-Time Setup

When you first launch the application:

1. **Add Widgets**
   - Menu: `Widgets` → `Add Video Stream`
   - Menu: `Widgets` → `Add Command Control`
   - Menu: `Widgets` → `Add Sensor Data`

2. **Arrange Your Workspace**
   - Drag widget title bars to reposition
   - Drag widgets outside to make them floating
   - Resize by dragging widget edges
   - Layouts are persisted automatically across sessions

3. **Connect to a Robot (if available)**
   - Menu: `ROS2` → `Connect`
   - Or click "Connect" in the top app bar

4. **Test with the Built-in Simulation**
   - Menu: `Simulation` → `Start Simulation`

### 5. Common Operations

#### Controlling the Robot

1. Open Command & Control widget.
2. Use sliders to set velocity:
   - **Linear Speed:** -1.0 to 1.0 m/s
   - **Angular Speed:** -π to π rad/s
3. Click **STOP** for a normal stop.
4. Click **EMERGENCY STOP** for an immediate halt.

#### Viewing Video Streams

1. Open the Video Stream widget.
2. Select a camera topic from the dropdown.

#### Monitoring Sensors

1. Open the Sensor Data widget — data updates automatically.

#### Digital Twin

1. Select a mode from the dropdown:
   - **Synchronized:** mirror real robot state
   - **Simulated:** internal physics simulation
   - **Offline:** disconnected
2. View real-time state information.

### 6. Custom Robot Model

The 3D viewport will load any `robot.obj` you drop next to the executable (or in the project root). Optionally include a matching `robot.mtl` and a `robot.png` texture. If no custom model is found, the bundled `box.obj` placeholder is shown.

### 7. Troubleshooting

#### Build Errors

**Problem:** "Qt6 not found"
```bash
sudo apt install qt6-base-dev
```

**Problem:** "ROS2 not found"
```bash
# Solution: Source ROS2, then rebuild
source /opt/ros/humble/setup.bash
./build.sh clean
```

#### Runtime Errors

**Problem:** "ROS2 interface not initialized"
- Ensure ROS2 is sourced before running.
- Check ROS2 is running: `ros2 topic list`.

**Problem:** "No video stream"
- Check the configured camera topic: `ros2 topic echo camera/raw`.
- Verify your camera node is publishing.

**Problem:** "Application crashes on start"
- Check log file: `cat OpenRobotControl.log`
- Try a debug build: `./build.sh clean debug`

### 8. Development Workflow

```bash
# Debug build during development
./build.sh clean debug

# Run with verbose output
./build/OpenRobotControl 2>&1 | tee output.log
```

### 9. ROS2 Topics Reference (code-accurate)

Subscribed:
- `camera/raw` (`sensor_msgs/msg/Image`)
- `/imu/data` (`sensor_msgs/msg/Imu`)
- `/robot_status` (`std_msgs/msg/String`)
- `/coordinates` (`geometry_msgs/msg/PointStamped`)
- `image/coordinates` (`std_msgs/msg/String`)

Published:
- `/cmd_vel` (`geometry_msgs/msg/Twist`)
- `/robot_command` (`std_msgs/msg/String`)

### 10. Next Steps

- Explore the modular widget system
- Add custom ROS2 topics
- Drop in your own `robot.obj` model
- Build platform-specific widgets and share them back upstream

## Getting Help

- Check the main `README.md` for detailed documentation.
- Review existing widgets under `src/ui/widgets/` for examples.
- ROS2 documentation: https://docs.ros.org/
- Qt documentation: https://doc.qt.io/

---

Open-source robot UI — happy hacking!
