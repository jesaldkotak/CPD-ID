import time
import struct
import os
import csv
from arduino.app_utils import App, Bridge

def read_sensor_csv(filename):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(script_dir, filename)
    data = []
    
    print(f"Attempting to read: {file_path}")
    if not os.path.exists(file_path):
        print(f"ERROR: File not found at {file_path}")
        return []

    try:
        with open(file_path, 'r') as f:
            content = f.read().replace(',', ' ').splitlines()
            for line in content:
                parts = line.split()
                if len(parts) >= 2:
                    try:
                        val = float(parts[1])
                        data.append(val)
                    except ValueError:
                        continue 
        return data[:2000]
    except Exception as e:
        print(f"Read Error: {e}")
        return []

def main():
    floats = read_sensor_csv("x_2026.csv")
    
    if not floats:
        print("Data loading failed. Stopping.")
        return

    chunk_size = 50 
    print(f"Streaming {len(floats)} points in chunks of {chunk_size}...")

    try:
        for i in range(0, len(floats), chunk_size):
            chunk = floats[i : i + chunk_size]
            payload = struct.pack(f'{len(chunk)}f', *chunk)
            Bridge.notify("stream_data", payload)
            time.sleep(0.02) 

        print("Processing...")
        response = Bridge.call("compute_and_get", len(floats), timeout=30)
        
        if response:
            num_res = len(response) // 4
            results = struct.unpack(f'{num_res}f', response)
            print(f"Success! Received {len(results)} results.")
            
            # --- NEW CSV WRITING LOGIC ---
            output_filename = "arduino_cusum_results.csv"
            with open(output_filename, 'w', newline='') as out_csv:
                writer = csv.writer(out_csv)
                writer.writerow(["Index", "Arduino_CUSUM"])
                for idx, val in enumerate(results):
                    # Using repr() to ensure we don't truncate the float representation prematurely
                    writer.writerow([idx + 1, repr(val)])
            
            print(f"Arduino results successfully saved to {output_filename}")
            print(f"First 5 data points: {results[0], results[1], results[2], results[3], }")
            
        else:
            print("Received no data back from Arduino.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run()