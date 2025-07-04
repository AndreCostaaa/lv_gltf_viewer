#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <output_path>"
    exit 1
fi

# Parameters
OUTPUT_PATH="/var/$1"

# Check if the RAM drive exists
if mountpoint -q "$OUTPUT_PATH"; then
    echo "The RAM drive exists at $OUTPUT_PATH."
    exit 0  # Return 0 if it exists
else
    echo "The RAM drive does not exist at $OUTPUT_PATH."
    exit 1  # Return 1 if it does not exist
fi
