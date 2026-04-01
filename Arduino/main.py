import time
import struct
import os
import csv
import re
from arduino.app_utils import App, Bridge

def read_multi_column_csv(filename):
    script_dir = os.path.dirname(os.path.abspath(__file__))    
    file_path = os.path.join(script_dir, filename)             
    
    if not os.path.exists(file_path):                          
        print(f"ERROR: File not found at {file_path}")
        return None, None

    columns_data = {}
    
    try:
        with open(file_path, 'r') as f:
            # Clean and parse the header line
            header_line = f.readline().strip()
            # Split by any whitespace or comma, remove quotes
            headers = [h.strip('"').strip() for h in re.split(r'[,\s]+', header_line) if h.strip()]
            
            # Remove the index column if it exists (e.g., if it starts with a number or is empty)
            if headers and (headers[0].isdigit() or not headers[0].lower().startswith('x')):
                headers = headers[1:]

            for h in headers:
                columns_data[h] = []

            # Parse all data rows
            for line in f:
                parts = [p.strip() for p in re.split(r'[,\s]+', line.strip()) if p.strip()]
                if not parts: continue
                
                # If there are more parts than headers, the first is the row index (1, 2, 3...)
                data_values = parts[1:] if len(parts) > len(headers) else parts
                
                for i, val in enumerate(data_values):
                    if i < len(headers):
                        try:
                            columns_data[headers[i]].append(float(val))
                        except ValueError:
                            continue
        
        # Ensure we only take the first 2000 points per column 
        for h in list(columns_data.keys()):
            columns_data[h] = columns_data[h][:2000]
            
        return headers, columns_data
    except Exception as e:
        print(f"Read Error: {e}")
        return None, None

def main():
    headers, all_data = read_multi_column_csv("x_100_2026.csv")            
    
    if not all_data or not headers:                                    
        print("Data loading failed. Check CSV format.")
        return

    print(f"Found {len(headers)} columns. Starting batch processing...")
    results_dict = {}
    
    # Lower chunk size to 25 because we are sending 8-byte doubles
    chunk_size = 25                                   

    for idx, col in enumerate(headers):
        x_data = all_data[col]
        print(f"[{idx+1}/{len(headers)}] Processing: {col}...")

        try:             
            for i in range(0, len(x_data), chunk_size):            
                chunk = x_data[i : i + chunk_size]
                
                # Pack as 'd' for double-precision float
                payload = struct.pack(f'{len(chunk)}d', *chunk)    
                Bridge.notify("stream_data", payload)              
                time.sleep(0.01) 

            response = Bridge.call("compute_and_get", len(x_data), timeout=80)    
            
            if response:
                # We unpack as 'i' (integers) because the Arduino is sending back
                # an array of 'int detected_cpts[]'
                num_res = len(response) // 4                        
                cpts = list(struct.unpack(f'{num_res}i', response))    
                results_dict[col] = cpts
                print(f"   Done. {len(cpts)} CPs found.")
            else:
                results_dict[col] = []
                print(f"   Done. No CPs found.")
                
            time.sleep(0.1)

        except Exception as e:
            print(f"   Error on {col}: {e}")
            results_dict[col] = ["ERROR"]

    save_results(headers, results_dict)

def save_results(headers, results_dict):
    filename = "detected_change_points_p10.csv"
    # Find the maximum number of CPs in any column to determine row count
    max_rows = 0
    for col in results_dict:
        if isinstance(results_dict[col], list):
            max_rows = max(max_rows, len(results_dict[col]))
    
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers) # Write x1, x2, x3... as header
        
        for i in range(max_rows):
            row = []
            for col in headers:
                col_res = results_dict.get(col, [])
                if i < len(col_res):
                    row.append(col_res[i])
                else:
                    row.append("") # Pad empty cells
            writer.writerow(row)
            
    print(f"\nSUCCESS: Results for {len(headers)} columns saved to {filename}")

if __name__ == "__main__":
    # In some bridge environments, logic must run after App.run() 
    # or as a task. If your setup supports it, we run main() then App.run().
    main()
    App.run()
