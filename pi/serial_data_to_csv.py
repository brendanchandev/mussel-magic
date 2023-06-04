import serial
import json
import pandas as pd
from collections import defaultdict

# replace '/dev/ttyACM0' with your Arduino's port
ser = serial.Serial('/dev/ttyACM0', 9600)

data = defaultdict(list)

try:
    while True:
        try:
            # Read the line from serial
            line = ser.readline().decode('utf-8').strip()

            # Parse the JSON data
            msg = json.loads(line)

            # Append the data to the appropriate list
            data['temperature'].append(msg['temperature'])
            data['ppm'].append(msg['ppm'])

            # Append the accel data
            for i in range(8):  # as we have 8 accelerometer data points (4 from flex sensors and 4 from ads1115)
                data['accel_data'].append(msg['accel_data'][i])

        except json.JSONDecodeError:
            print("Received malformed JSON. Skipping...")

except KeyboardInterrupt:
    print("Stopping program")

# Save all data to separate CSV files
for key in data.keys():
    df = pd.DataFrame(data[key], columns=[key])
    df.to_csv(f"../data/{key}.csv", index=False)