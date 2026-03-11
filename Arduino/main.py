import time
import struct
import os
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
        return data[:2000] # Ensure we don't exceed the MCU buffer
    except Exception as e:
        print(f"Read Error: {e}")
        return []

def main():
    x_data = read_sensor_csv("x_2026.csv")            
    
    if not x_data:                                    
        print("Data loading failed. Stopping.")
        return

    chunk_size = 50                                   
    print(f"Streaming {len(x_data)} points to the Arduino in chunks of {chunk_size}...")

    try:             
        for i in range(0, len(x_data), chunk_size):            
            chunk = x_data[i : i + chunk_size]
            payload = struct.pack(f'{len(chunk)}f', *chunk)    
            Bridge.notify("stream_data", payload)              
            time.sleep(0.02)                                   

        print("Sent data to MCU. Executing PCM_TH algorithm...")
        
        # We increase the timeout because 2000 points take time to calculate!
        response = Bridge.call("compute_and_get", len(x_data), timeout=60)    
        
        if response:
            num_res = len(response) // 4                        
            results = struct.unpack(f'{num_res}i', response)    
            
            print(f"\n--- MCU COMPUTATION COMPLETE ---")
            print(f"Detected Change Points: {list(results)}")
            print("--------------------------------\n")
            
        else:
            print("No change points were detected by the Arduino.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run()