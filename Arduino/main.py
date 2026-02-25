import time
import struct
import os
import csv
from arduino.app_utils import App, Bridge

def read_sensor_csv(filename):
    script_dir = os.path.dirname(os.path.abspath(__file__))    #safely locating the pwd
    file_path = os.path.join(script_dir, filename)             #assuming that the file is under the python pwd
    data = []
    
    print(f"Attempting to read: {file_path}")                  #checking if file exists
    if not os.path.exists(file_path):                          
        print(f"ERROR: File not found at {file_path}")
        return []

    try:
        with open(file_path, 'r') as f:                        
            content = f.read().replace(',', ' ').splitlines()    #reading csv
            for line in content:                                 #seeing if it has 2 columns or 1
                parts = line.split()
                if len(parts) >= 2:
                    try:
                        val = float(parts[1])                    #data value
                        data.append(val)                         #append in the data array
                    except ValueError:
                        continue 
        return data[:2000]
    except Exception as e:
        print(f"Read Error: {e}")
        return []

def write_results_csv(results_store):
    output_filename = "arduino_cusum_results.csv"
    with open(output_filename, 'w', newline='') as out_csv:    #writing in the csv file, why not make a new function just like in read case?
        writer = csv.writer(out_csv)
        writer.writerow(["Index", "Arduino_CUSUM"])
        for idx, val in enumerate(results_store):
            writer.writerow([idx + 1, repr(val)])    # repr() ensures we don't truncate the float representation prematurely
    
    print(f"Arduino results successfully saved to {output_filename}")
    
            

def main():    #store data in buffer, send it to MCU stream in chunks, initiate MCU function calc, write the results in a new csv file, check for errors
    x_data = read_sensor_csv("x_2026.csv")            #store the data in MPU buffer
    
    if not x_data:                                    #check if data file is not empty
        print("Data loading failed. Stopping.")
        return

    chunk_size = 50                                   #found this through trial and error, can be optimised later
    print(f"Streaming {len(x_data)} points in chunks of {chunk_size}...")

    try:             #Try to run this communication code, but if anything goes wrong, don't crash the whole program. Instead, jump to the except block and print the error message e
        for i in range(0, len(x_data), chunk_size):            #for sending data to MCU in chunks of 50
            chunk = x_data[i : i + chunk_size]
            payload = struct.pack(f'{len(chunk)}f', *chunk)    #struct.pack converts Py numbers into standard 4-byte little-endian format
            #Python "floats" are complex objects that carry a lot of extra metadata. The Arduino is a simple 8-bit/32-bit machine that expects raw binary data.
            Bridge.notify("stream_data", payload)              #Sends data to MCU
            time.sleep(0.02)                                   #safe for not overrunning the buffer

        print("Sent data to MCU. Executing the function...")
        response = Bridge.call("compute_and_get", len(x_data), timeout=30)    #Initiates the cusum calculcation, timeout is the time given for MCU to respond, the code exits after that giving an error.
        
        if response:
            num_res = len(response) // 4                        #response is a raw byte vector. float = 4 bytes, len(response) is in bytes, num_res= how many floating-point numbers
            results = struct.unpack(f'{num_res}f', response)    #unpacking the result
            print(f"MCU Function Computation Complete!")

            write_results_csv(results)
            print(f"First 5 data points: {results[0], results[1], results[2], results[3], results[4]}")
            
        else:
            print("Received no data back from Arduino.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run()