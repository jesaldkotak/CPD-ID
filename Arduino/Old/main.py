import time
import struct
import os
import csv
import re
from arduino.app_utils import App, Bridge

# ==========================================
# TOGGLE THIS FLAG TO SWITCH MODES
# True = Run 1 column and save CUSUM logs
# False = Run 100 columns and save Change Points
DEBUG_MODE = False 
# ==========================================

def read_multi_column_csv(filename):
    script_dir = os.path.dirname(os.path.abspath(__file__))    
    file_path = os.path.join(script_dir, filename)             
    
    if not os.path.exists(file_path):                          
        print(f"ERROR: File not found at {file_path}")
        return None, None

    columns_data = {}
    
    try:
        with open(file_path, 'r') as f:
            header_line = f.readline().strip()
            headers = [h.strip('"').strip() for h in re.split(r'[,\s]+', header_line) if h.strip()]
            if headers and (headers[0].isdigit() or not headers[0].lower().startswith('x')):
                headers = headers[1:]

            for h in headers:
                columns_data[h] = []

            for line in f:
                parts = [p.strip() for p in re.split(r'[,\s]+', line.strip()) if p.strip()]
                if not parts: continue
                data_values = parts[1:] if len(parts) > len(headers) else parts
                
                for i, val in enumerate(data_values):
                    if i < len(headers):
                        try:
                            columns_data[headers[i]].append(float(val))
                        except ValueError:
                            continue
        
        for h in list(columns_data.keys()):
            columns_data[h] = columns_data[h][:2000]
            
        return headers, columns_data
    except Exception as e:
        print(f"Read Error: {e}")
        return None, None

def run_debug_mode(headers, all_data):
    print("--- DEBUG MODE ACTIVE ---")
    
    # Just type the exact column name you want to test here!
    col_name = "x40" 
    
    x_data = all_data[col_name]
    print(f"Streaming {len(x_data)} points for column {col_name}...")

    chunk_size = 50                                   
    for i in range(0, len(x_data), chunk_size):            
        chunk = x_data[i : i + chunk_size]
        payload = struct.pack(f'{len(chunk)}f', *chunk)    
        Bridge.notify("stream_data", payload)              
        time.sleep(0.01)

    print("Fetching CUSUM calculation logs from Arduino...")
    response = Bridge.call("run_and_log_cusum", len(x_data), timeout=80)    
    
    if response:
        # Unpacking jumps from 4 to 7
        num_floats = len(response) // 4                        
        logs = struct.unpack(f'{num_floats}f', response)    
        
        with open("arduino_debug_log.csv", "w", newline="") as f:
            writer = csv.writer(f)
            # Added new column headers to match R
            writer.writerow(["Direction", "Exp_Interval", "Start_Index", "End_Index", "Candidate_Index", "Max_CUSUM", "Order"])
            
            # Step by 7 
            for i in range(0, len(logs), 7):
                
                # Check for our Special System Message flag
                if logs[i] == 0.0:
                    print(f"   -> Arduino thr_fin calculated as: {logs[i+5]:.7f}")
                    continue 

                # Map the 7 variables
                direction = "Right" if logs[i] > 0 else "Left"
                exp_interval = int(logs[i+1])
                start_index = int(logs[i+2])
                end_index = int(logs[i+3])
                cand_index = int(logs[i+4])
                max_cusum = logs[i+5]
                order = int(logs[i+6])
                
                writer.writerow([direction, exp_interval, start_index, end_index, cand_index, f"{max_cusum:.7f}", order])
                
        print(f"SUCCESS: Saved {(len(logs)//7)-1} log entries to arduino_debug_log.csv")
    else:
        print("Timeout or empty response from Arduino.")

def run_production_mode(headers, all_data):
    print(f"--- PRODUCTION MODE ACTIVE ---")
    print(f"Found {len(headers)} columns. Starting batch processing...")
    results_dict = {}
    chunk_size = 50                                   

    for idx, col in enumerate(headers):
        x_data = all_data[col]
        print(f"[{idx+1}/{len(headers)}] Processing: {col}...")

        try:             
            for i in range(0, len(x_data), chunk_size):            
                chunk = x_data[i : i + chunk_size]
                payload = struct.pack(f'{len(chunk)}f', *chunk)    
                Bridge.notify("stream_data", payload)              
                time.sleep(0.01)

            response = Bridge.call("compute_and_get", len(x_data), timeout=80)    
            
            if response:
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
    filename = "ard_cpt_ds4_p10.csv"
    max_rows = 0
    for col in results_dict:
        if isinstance(results_dict[col], list):
            max_rows = max(max_rows, len(results_dict[col]))
    
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers) 
        
        for i in range(max_rows):
            row = []
            for col in headers:
                col_res = results_dict.get(col, [])
                if i < len(col_res):
                    row.append(col_res[i])
                else:
                    row.append("") 
            writer.writerow(row)
            
    print(f"\nSUCCESS: Results for {len(headers)} columns saved to {filename}")

def main():
    # Make sure this matches your actual data file name!
    headers, all_data = read_multi_column_csv("x_100_2026_ds4.csv")            
    
    if not all_data or not headers:                                    
        print("Data loading failed. Check CSV format.")
        return

    if DEBUG_MODE:
        run_debug_mode(headers, all_data)
    else:
        run_production_mode(headers, all_data)

if __name__ == "__main__":
    main()
    App.run()
