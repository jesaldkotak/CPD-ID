import time
import struct
import os
import csv
from arduino.app_utils import App, Bridge

def read_sensor_csv(filename, target_column=16):
    """
    Reads data and extracts the specific column for x16.
    target_column=16 assumes: [Index, x1, x2, ..., x16]
    """
    script_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(script_dir, filename)
    data = []
    
    print(f"Attempting to read: {file_path}")
    if not os.path.exists(file_path):                          
        print(f"ERROR: File not found at {file_path}")
        return []

    try:
        with open(file_path, 'r') as f:
            # We replace commas with spaces to normalize CSV vs space-separated formats
            content = f.read().replace(',', ' ').splitlines()
            
            for line in content:
                parts = line.split()
                
                # Check if the row has enough columns (e.g., at least 18 columns for index 16)
                if len(parts) > target_column:
                    try:
                        # Attempt to grab the x16 value
                        val = float(parts[target_column])
                        data.append(val)
                    except ValueError:
                        # This skips the header (like 'x16') or empty strings
                        continue 
        
        if not data:
            print(f"Warning: No numeric data found in column index {target_column}.")
            print("Check if the file has at least 18 columns (including the row index).")
        else:
            print(f"Successfully loaded {len(data)} points from column x{target_column}.")

        print(data[1])
        return data[:2000]
    except Exception as e:
        print(f"Read Error: {e}")
        return []

def write_results_csv(results_store):
    output_filename = "arduino_cusum_x16_results.csv"
    with open(output_filename, 'w', newline='') as out_csv:
        writer = csv.writer(out_csv)
        writer.writerow(["Index", "Arduino_CUSUM_x16"])
        for idx, val in enumerate(results_store):
            writer.writerow([idx + 1, repr(val)])
    
    print(f"Arduino results successfully saved to {output_filename}")

def main():
    # We target column index 16 to get x16 data
    x_data = read_sensor_csv("x_100_2026.csv", target_column=16)
    
    if not x_data:
        print("Data loading failed. Stopping.")
        return

    chunk_size = 50
    print(f"Streaming x16 data ({len(x_data)} points) in chunks of {chunk_size}...")

    try:
        for i in range(0, len(x_data), chunk_size):
            chunk = x_data[i : i + chunk_size]
            payload = struct.pack(f'{len(chunk)}f', *chunk)
            Bridge.notify("stream_data", payload)
            time.sleep(0.02)

        print("Sent data to MCU. Executing computation...")
        response = Bridge.call("compute_and_get", len(x_data), timeout=30)
        
        if response:
            num_res = len(response) // 4
            results = struct.unpack(f'{num_res}f', response)
            print(f"MCU Function Computation Complete!")

            write_results_csv(results)
            # Display first 5 results
            d_limit = min(5, len(results))
            print(f"First {d_limit} result points: {results[:d_limit]}")
            
        else:
            print("Received no data back from Arduino.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run()
