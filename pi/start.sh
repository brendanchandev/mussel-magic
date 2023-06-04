#!/bin/bash

sudo apt-get update

sudo apt-get install rpi.gpio

# Name of your Python scripts
PYTHON_SCRIPT_SERIAL="serial_data_to_csv.py"
PYTHON_SCRIPT_VIDEO="record.py"
PYTHON_SCRIPT_UV="uv.py"

# Create a Python virtual environment
python3 -m venv venv

# Activate the virtual environment
source venv/bin/activate

# Install necessary Python packages
pip install -r requirements.txt

# Run your Python scripts in the background
python $PYTHON_SCRIPT_SERIAL &
SERIAL_PID=$!

python $PYTHON_SCRIPT_VIDEO &
VIDEO_PID=$!

python $PYTHON_SCRIPT_UV &
UV_PID=$!

# Function to handle interrupt signal
trap 'kill $SERIAL_PID; kill $VIDEO_PID; kill $UV_PID' INT

# Wait for all scripts to finish
wait $SERIAL_PID
wait $VIDEO_PID
wait $UV_PID

# Deactivate the virtual environment after the scripts are done running
deactivate
