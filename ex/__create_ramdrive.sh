#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <output_path (without /var prefix, that is always added)> <size_descriptor (ex 1M for one megabyte)>"
    exit 1
fi

# Parameters
OUTPUT_PATH="/var/$1"
SIZE_DESCRIPTOR="$2"

# Line to be added to /etc/fstab
FSTAB_LINE="tmpfs $OUTPUT_PATH tmpfs nodev,nosuid,size=$SIZE_DESCRIPTOR 0 0"

# Check if the line already exists in /etc/fstab
if grep -qF "$FSTAB_LINE" /etc/fstab; then
    echo "The line already exists in /etc/fstab. No changes made."
else
    # Append the line to /etc/fstab
    echo "$FSTAB_LINE" | sudo tee -a /etc/fstab > /dev/null
    echo "Added the line to /etc/fstab.  Reloading systemctl daemon and updating mount points."
    sudo systemctl daemon-reload
    sudo mount -a
    df
fi
