import os
import serial
import json
import pandas as pd
import signal
import sys
from collections import deque
import time

ser = serial.Serial('/dev/ttyUSB0', 9600)

data = {"temperature": deque(maxlen=20), "ppm": deque(maxlen=20), "accel_data": deque(maxlen=20)}

def cleanup(signum, frame):
    # This function will be called when the script receives a SIGTERM signal
    print("Received SIGTERM, performing cleanup and exiting...")
    # Perform any necessary cleanup here
    sys.exit(0)

signal.signal(signal.SIGTERM, cleanup)

first_read = True

while True:
    try:
        if ser.inWaiting():
            if first_read:
                first_read = False
                continue
            
            jsonData = ser.readline()
            jsonData = jsonData.decode('utf-8', errors='replace').strip()
            jsonData = json.loads(jsonData)

            data["temperature"].append(jsonData["temperature"])
            data["ppm"].append(jsonData["ppm"])
            data["accel_data"].append(jsonData["accel_data"])
            
            timestamp = time.time()
            
            # Append data to csv files
            for key in data.keys():
                file_exists = os.path.isfile(f"../data/{key}.csv")
                if key != 'accel_data':
                    with open(f"../data/{key}.csv", 'a') as f:
                        df = pd.DataFrame(list(data[key]))
                        df.insert(0, 'timestamp', timestamp)  # Insert Unix timestamp as first column
                        df.to_csv(f, header=not file_exists, index=False)
                else:
                    # For accel_data, each row in csv should correspond to one value of accel_data
                    with open(f"../data/{key}.csv", 'a') as f:
                        df = pd.DataFrame(pd.Series(data[key]).apply(pd.Series))
                        df.insert(0, 'timestamp', timestamp)
                        df.to_csv(f, mode='a', header=not file_exists, index=False)

    except json.JSONDecodeError as e:
        print(f"JSONDecodeError: {e}")
    except UnicodeDecodeError as e:
        print(f"UnicodeDecodeError: {e}")
