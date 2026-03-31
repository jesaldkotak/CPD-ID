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
    output_filename = "arduino_cusum_x16_results_v2.csv"
    with open(output_filename, 'w', newline='') as out_csv:
        writer = csv.writer(out_csv)
        writer.writerow(["Index", "Arduino_CUSUM_x16"])
        for idx, val in enumerate(results_store):
            writer.writerow([idx + 1, repr(val)])
    
    print(f"Arduino results successfully saved to {output_filename}")

def main():
    x_data = read_sensor_csv("x_100_2026.csv", target_column=16)
    if not x_data: return

    chunk_size = 25  # Reduced chunk size because doubles are twice as large
    print(f"Streaming x16 as doubles in chunks of {chunk_size}...")

    try:
        for i in range(0, len(x_data), chunk_size):
            chunk = x_data[i : i + chunk_size]
            # 'd' is the format for an 8-byte double-precision float
            payload = struct.pack(f'{len(chunk)}d', *chunk)
            Bridge.notify("stream_data", payload)
            time.sleep(0.02)

        print("Executing computation...")
        response = Bridge.call("compute_and_get", len(x_data), timeout=30)
        
        if response:
            # Each double is 8 bytes
            num_res = len(response) // 8
            results = struct.unpack(f'{num_res}d', response)
            
            write_results_csv(results)
            print(f"Success! Received {num_res} double-precision results.")
        else:
            print("No response from Arduino.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run()
