#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <output_path (without /var prefix, that is always added)> <size_descriptor (ex 1M for one megabyte)>"
    exit 1
fi

# Parameters
OUTPUT_PATH="/var/$1"
SIZE_DESCRIPTOR="$2"

# Line to be checked in /etc/fstab
FSTAB_LINE="tmpfs $OUTPUT_PATH tmpfs nodev,nosuid,size=$SIZE_DESCRIPTOR 0 0"


# Check if the goal size is already the actual size
if grep -qF "$FSTAB_LINE" /etc/fstab; then
    echo "The RAM drive already exists and is the specified size."
else
    # Check if the RAM drive exists
    if mountpoint -q "$OUTPUT_PATH"; then
        echo "The RAM drive exists at $OUTPUT_PATH."

        # Resize the RAM drive
        echo "Resizing the RAM drive to size $SIZE_DESCRIPTOR."
        sudo mount -o remount,size=$SIZE_DESCRIPTOR "$OUTPUT_PATH"
        echo "The RAM drive has been resized."

        # Update /etc/fstab to reflect the new size
        sudo sed -i "s|^tmpfs $OUTPUT_PATH tmpfs nodev,nosuid,size=.*|$FSTAB_LINE|" /etc/fstab
        echo "Updated /etc/fstab to reflect the new size. Reloading systemctl daemon."
        sudo systemctl daemon-reload
        df
    else
        echo "The RAM drive does not exist at $OUTPUT_PATH."

        # Call the script to create a new RAM drive
        echo "Creating a new RAM drive."
        ./__create_ramdrive.sh "$1" "$SIZE_DESCRIPTOR"
        
        # Create the RAM drive
        sudo mount -a
        echo "The RAM drive has been created and mounted."
    fi
fi