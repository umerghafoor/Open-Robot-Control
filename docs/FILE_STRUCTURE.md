# Project File Structure

```text
OpenRobotControl/
│
├── CMakeLists.txt                 # Main build configuration
├── package.xml                    # ROS2 package manifest
├── LICENSE                        # MIT license
├── README.md                      # Main documentation
├── CLAUDE.md                      # Contributor / AI assistant guide
├── build.sh                       # Build script
├── run.sh                         # Launcher
├── install_dependency.sh          # Install Ubuntu build dependencies
├── Dockerfile                     # ROS2 + Qt6 Docker base image
├── build_docker.sh                # Build the Docker image
├── start_docker.sh                # Launch a container with X11 + ROS2
├── theme.qss                      # Qt stylesheet (light theme)
├── resources.qrc                  # Qt resources manifest
├── box.obj                        # Bundled sample 3D model
│
├── src/                           # Source code
│   │
│   ├── main.cpp                   # Application entry point
│   │
│   ├── core/                      # Core framework
│   │   ├── Application.h/.cpp     # Lifecycle facade
│   │   └── WidgetManager.h/.cpp   # Widget factory & registry
│   │
│   ├── ros2/                      # ROS2 integration module
│   │   └── ROS2Interface.h/.cpp   # Publishers, subscribers, threaded spin
│   │
│   ├── twin/                      # Digital Twin module
│   │   ├── DigitalTwin.h/.cpp     # Mode management
│   │   ├── TwinState.h/.cpp       # Pose, velocity, sensors
│   │   └── TwinSimulator.h/.cpp   # Kinematics, sensor sim
│   │
│   ├── ui/                        # User interface
│   │   ├── MainWindow.h/.cpp      # Dockable layout, menu, toolbar
│   │   ├── MaterialDockWidget.*   # Custom dock widget
│   │   └── widgets/               # Modular dockable widgets
│   │       ├── BaseWidget.*          # Abstract base
│   │       ├── VideoStreamWidget.*   # Camera feeds
│   │       ├── CommandControlWidget.* # Robot control + e-stop
│   │       ├── MotionControlWidget.* # Velocity sliders
│   │       ├── SensorDataWidget.*    # Tabular sensor display
│   │       ├── CoordinatesWidget.*   # Coordinate readouts
│   │       ├── CurrentDetectionWidget.*
│   │       ├── DetectionSummaryWidget.*
│   │       ├── DetectionPanelWidget.*
│   │       ├── TwinVisualizationWidget.*
│   │       ├── RobotMapView.*
│   │       ├── StatusBadge.*
│   │       ├── ZoomableImageView.*
│   │       ├── RobotModelWidget.*    # 3D model viewport
│   │       ├── LaserCalibrationWidget.*
│   │       ├── IMU3DWidget.*
│   │       └── SidebarWidget.*
│   │
│   ├── rendering/                 # OpenGL / OBJ rendering helpers
│   │   ├── OBJMesh.h
│   │   ├── OBJLoader.h/.cpp
│   │   └── ModelGLView.h/.cpp
│   │
│   └── utils/
│       └── Logger.h/.cpp          # Thread-safe singleton logger
│
├── docs/                          # Documentation
│   ├── ARCHITECTURE.md
│   ├── DIAGRAMS.md
│   ├── DOCUMENTATION_INDEX.md
│   ├── FILE_STRUCTURE.md          # (this file)
│   ├── PROJECT_SUMMARY.md
│   ├── QUICKSTART.md
│   ├── ROS2-Communication.md
│   └── ROS2_NODES.md
│
├── build/                         # Build directory (generated)
│   └── OpenRobotControl           # Executable (after build)
│
└── log/                           # Build logs (generated)
```

## Module Dependencies

```text
main.cpp
  └─→ Application
       ├─→ ROS2Interface
       │    └─→ rclcpp (ROS2)
       │
       ├─→ DigitalTwin
       │    ├─→ TwinState
       │    ├─→ TwinSimulator
       │    └─→ ROS2Interface
       │
       └─→ MainWindow
            ├─→ WidgetManager
            │    └─→ BaseWidget
            │         └─→ (all concrete widgets)
            │
            ├─→ ROS2Interface
            └─→ DigitalTwin

Utils (Logger) ─→ Used by all modules
```

## Key Design Decisions

### Separation of Concerns

- ROS2 code isolated in `src/ros2/`.
- Digital Twin logic in `src/twin/`.
- UI completely separate in `src/ui/`.

### Modularity

- Each widget is independent.
- Factory pattern for widget creation.
- Easy to add new widgets and topics.

### Thread Safety

- ROS2 spinning runs in a dedicated `QThread`.
- Qt signals/slots for cross-thread communication.
- Mutex protection in `Logger`.

### Optional Dependencies

- ROS2 is optional. The app builds and runs in standalone (stub) mode when ROS2 is not sourced.
- Qt Multimedia and Qt OpenGL are detected at configure time and enable optional features.

## Build Artifacts

After building you will have:

```text
build/
├── OpenRobotControl              # Main executable
├── CMakeCache.txt                # CMake cache
├── CMakeFiles/                   # CMake internals
└── Makefile                      # Generated Makefile
```

## Runtime Artifacts

When running the application:

```text
OpenRobotControl.log              # Application log file
```
