#!/bin/bash

if loginctl show-session 1 | grep x11; then
    # Session 1 is Type X11
    echo "x11"
    exit 0 
fi
if pgrep -x wayfire > /dev/null; then
    # Session 1 is Type Wayland running Wayfire
    echo "wayland + wayfire"
    exit 1
fi
if pgrep -x labwc > /dev/null; then
    # Session 1 is Type Wayland running labwc
    echo "wayland + labwc"
    exit 2
fi

echo "unknown"
exit 99