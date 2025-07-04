#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <output_path (without /var prefix, that is always added)> <size_descriptor (ex 1M for one megabyte)>"
    exit 1
fi

# Parameters
OUTPUT_PATH="/var/$1"
SIZE_DESCRIPTOR="$2"

# Line to be checked and potentially removed from /etc/fstab
FSTAB_LINE="tmpfs $OUTPUT_PATH tmpfs nodev,nosuid,size=$SIZE_DESCRIPTOR 0 0"

# Check if the line exists in /etc/fstab
if grep -qF "$FSTAB_LINE" /etc/fstab; then
    echo "The following line exists in /etc/fstab:"
    echo "$FSTAB_LINE"
    
    # Prompt the user for confirmation
    read -p "Do you want to remove this line? (y/n): " confirm
    if [[ "$confirm" =~ ^[Yy]$ ]]; then
        # Remove the line from /etc/fstab
        sudo sed -i "\|$FSTAB_LINE|d" /etc/fstab
        echo "The line has been removed from /etc/fstab.  Reloading systemctl daemon and updating mount points."
        sudo systemctl daemon-reload
        sudo umount $OUTPUT_PATH
        df
    else
        echo "No changes made."
    fi
else
    echo "The specified line does not exist in /etc/fstab."
fi
