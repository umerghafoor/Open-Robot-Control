# ROS2 Topics and Commands (OpenRobotControl)

This document lists ROS2 topics used by OpenRobotControl and common `ros2` CLI commands to inspect them.

## Topics used by the application

- `/cmd_vel` (`geometry_msgs/msg/Twist`)
  - Purpose: Velocity command published by control widgets (`linear.x`, `angular.z` for differential-drive).
  - Publisher: OpenRobotControl (`CommandControlWidget`, `MotionControlWidget`).
  - QoS: default (10).

- `/robot_command` (`std_msgs/msg/String`)
  - Purpose: Higher-level string commands such as `EMERGENCY_STOP` or custom payloads.
  - Publisher: OpenRobotControl (`CommandControlWidget`).

- `/robot_status` (`std_msgs/msg/String`)
  - Purpose: Status or telemetry messages from the robot (string payload).
  - Subscriber: OpenRobotControl (`ROS2Interface`).

- `camera/raw` (`sensor_msgs/msg/Image`)
  - Purpose: Raw camera images for the Video Stream widget.
  - Subscriber: OpenRobotControl (`ROS2Interface`).
  - Note: `bgr8` frames are converted to RGB before being emitted to Qt.

- `/imu/data` (`sensor_msgs/msg/Imu`)
  - Purpose: IMU data for sensor visualization.
  - Subscriber: OpenRobotControl (`ROS2Interface`).

- `/coordinates` (`geometry_msgs/msg/PointStamped`)
  - Purpose: World/robot coordinate updates.
  - Subscriber: OpenRobotControl (`ROS2Interface`).

- `image/coordinates` (`std_msgs/msg/String`, JSON payload)
  - Purpose: Coordinates of detections in the image frame.
  - Subscriber: OpenRobotControl (`ROS2Interface`).

## Common ros2 CLI commands

Replace topic names as needed. These examples assume ROS2 is sourced and your `ROS_DOMAIN_ID` / RMW implementation are configured.

List topics:

```bash
ros2 topic list
```

Show topic type and info:

```bash
ros2 topic info /cmd_vel
ros2 topic type /cmd_vel
```

Echo messages on a topic:

```bash
ros2 topic echo /cmd_vel
ros2 topic echo /robot_status
```

Publish a test Twist (one-shot):

```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \
  "{ linear: { x: 0.5, y: 0.0, z: 0.0 }, angular: { x: 0.0, y: 0.0, z: 0.1 } }" -1
```

Publish a string command:

```bash
ros2 topic pub /robot_command std_msgs/msg/String "{ data: 'EMERGENCY_STOP' }" -1
```

Monitor topic bandwidth and rate (useful for image streams):

```bash
ros2 topic hz camera/raw
ros2 topic bw camera/raw
```

## Notes

- Topic names and message types are defined in `src/ros2/ROS2Interface.cpp` and the widget code under `src/ui/widgets/`. Treat the source as authoritative.
- If ROS2 is not available at build time, OpenRobotControl compiles in stub mode and will log publish actions instead of actually publishing messages.
- Want to add a topic or change names? See the "Extending" section in the main `README.md`.
