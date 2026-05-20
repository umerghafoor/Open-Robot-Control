#!/bin/bash

# OpenRobotControl - Launcher Script
# This script runs the application with proper output handling

cd "$(dirname "$0")"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== OpenRobotControl ===${NC}"
echo ""

# ─── Safe-mode defaults ──────────────────────────────────────────────────────
# When the GUI is hosted by a fragile compositor (Wayland) or shared via a
# container, a misbehaving GPU/GL client can take down gnome-shell on the host.
# These env vars force the app onto a conservative path that has the smallest
# blast radius on the host session.
#
#  - SAFE_MODE=1                            → force everything below
#  - inside Docker (detected)               → force everything below
#  - otherwise opt out per-knob if you like (e.g. FORCE_XCB=0, SOFTWARE_GL=0)

in_container=0
if [ -f /.dockerenv ] || grep -qE 'docker|containerd|kubepods' /proc/1/cgroup 2>/dev/null; then
    in_container=1
fi

if [ "${SAFE_MODE:-0}" = "1" ] || [ "$in_container" = "1" ]; then
    echo -e "${YELLOW}Safe mode enabled (container=${in_container}, SAFE_MODE=${SAFE_MODE:-0})${NC}"
    : "${FORCE_XCB:=1}"
    : "${SOFTWARE_GL:=1}"
fi

# Force XCB (X11) over Wayland — gnome-shell tolerates GL faults from X11 clients
# far better than from native Wayland clients.
#
# Qt accepts a fallback list separated by ';'. If xcb fails (e.g. missing
# X11 cookie inside a container) Qt will try offscreen instead of aborting.
# Set HEADLESS=1 to skip xcb entirely and start in offscreen mode.
if [ "${HEADLESS:-0}" = "1" ]; then
    export QT_QPA_PLATFORM=offscreen
    echo -e "${BLUE}  QT_QPA_PLATFORM=offscreen (HEADLESS=1)${NC}"
elif [ "${FORCE_XCB:-0}" = "1" ]; then
    export QT_QPA_PLATFORM='xcb;offscreen'
    echo -e "${BLUE}  QT_QPA_PLATFORM='xcb;offscreen' (offscreen fallback)${NC}"
fi

# Container-specific X11 sanity check: if DISPLAY is set but xauth lookup fails,
# warn the user before Qt aborts with a cryptic core-dump.
if [ "$in_container" = "1" ] && [ "${HEADLESS:-0}" != "1" ] && [ -n "$DISPLAY" ]; then
    if command -v xauth >/dev/null 2>&1; then
        if ! xauth list "$DISPLAY" >/dev/null 2>&1; then
            echo -e "${YELLOW}⚠ No X11 auth cookie for DISPLAY=$DISPLAY inside this container.${NC}"
            echo -e "${YELLOW}  On the host, run:  xhost +SI:localuser:\$(id -un)${NC}"
            echo -e "${YELLOW}  Or pass the host cookie:${NC}"
            echo -e "${YELLOW}    docker cp ~/.Xauthority <container>:\$HOME/.Xauthority${NC}"
            echo -e "${YELLOW}  Or run headless:  HEADLESS=1 ./run.sh${NC}"
        fi
    fi
fi

# Force software OpenGL (llvmpipe). Slower, but cannot crash the host GPU stack.
if [ "${SOFTWARE_GL:-0}" = "1" ]; then
    export LIBGL_ALWAYS_SOFTWARE=1
    export QT_OPENGL=software
    export QT_QUICK_BACKEND=software
    echo -e "${BLUE}  LIBGL_ALWAYS_SOFTWARE=1, QT_OPENGL=software${NC}"
fi

# Disable Qt's high-DPI scaling pixel-doubling in software paths (lighter on CPU)
export QT_AUTO_SCREEN_SCALE_FACTOR=0

# Check if executable exists
if [ ! -f "build/open_robot_control" ]; then
    echo -e "${YELLOW}Executable not found. Building...${NC}"
    ./build.sh release
    if [ $? -ne 0 ]; then
        echo -e "${RED}Build failed!${NC}"
        exit 1
    fi
fi

# Check if ROS2 is sourced
if [ -n "$ROS_DISTRO" ]; then
    echo -e "${GREEN}✓ ROS2 detected: $ROS_DISTRO${NC}"
else
    echo -e "${YELLOW}⚠ ROS2 not detected - Running in standalone mode${NC}"
fi

echo ""
echo -e "${GREEN}Starting application...${NC}"
echo "Log file: OpenRobotControl.log"
echo ""

# ─── Clean shutdown ──────────────────────────────────────────────────────────
# Run the app in the background so this shell stays responsive to signals.
# On SIGINT/SIGTERM, forward SIGTERM to the app (Qt translates to quit() and
# tears down ROS2 cleanly), then wait up to 5s before escalating to SIGKILL.
APP_PID=
shutdown() {
    if [ -n "$APP_PID" ] && kill -0 "$APP_PID" 2>/dev/null; then
        echo ""
        echo -e "${YELLOW}Stopping application (pid $APP_PID)...${NC}"
        kill -TERM "$APP_PID" 2>/dev/null
        for _ in 1 2 3 4 5; do
            kill -0 "$APP_PID" 2>/dev/null || break
            sleep 1
        done
        if kill -0 "$APP_PID" 2>/dev/null; then
            echo -e "${RED}Did not exit in time; sending SIGKILL${NC}"
            kill -KILL "$APP_PID" 2>/dev/null
        fi
    fi
}
trap shutdown INT TERM HUP

./build/open_robot_control "$@" &
APP_PID=$!
wait "$APP_PID"
EXIT_CODE=$?
APP_PID=
trap - INT TERM HUP

echo ""
echo -e "${BLUE}Application exited with code: $EXIT_CODE${NC}"

# Show last few log entries
if [ -f "OpenRobotControl.log" ]; then
    echo ""
    echo -e "${BLUE}Last log entries:${NC}"
    tail -n 10 OpenRobotControl.log
fi

exit $EXIT_CODE
