import time
import struct
from arduino.app_utils import App, Bridge

def main():
    print("Initializing Bridge...")
    time.sleep(2) # Give MCU time to register providers [cite: 65]
    
    print("Requesting PCM_TH Analysis from Arduino...")
    try:
        # Call the C++ function [cite: 65]
        response = Bridge.call("run_pcm_th_test")
        
        if response:
            # Each 'int' is 4 bytes. We determine how many points were found. [cite: 53]
            num_cpts = len(response) // 4
            # Unpack the little-endian binary data into a Python list
            cpts = struct.unpack(f'{num_cpts}i', response)
            
            print("\n--- ANALYSIS COMPLETE ---")
            print(f"Detected Change Points: {list(cpts)}")
            print("--------------------------\n")
        else:
            print("No change points were detected by the MCU.")
            
    except Exception as e:
        print(f"Communication error: {e}")

if __name__ == "__main__":
    main()
    App.run() # Keep the channel open [cite: 65]