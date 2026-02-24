import time
from arduino.app_utils import App, Bridge

def main():
    print("Initializing Bridge...")
    # No bridge.get_proxy() needed here
    
    # Wait a moment for the C++ side to register its functions
    time.sleep(2)
    
    print("Calling C++ run_calc...")
    try:
        # We use Bridge.call directly with the name of the function
        # provided in the C++ code: Bridge.provide("run_calc", run_calc);
        result = Bridge.call("run_calc")
        
        print("Success! Calculation finished on MCU.")
        print("\n--- CUSUM Results ---")
        print(result)
        print("---------------------\n")
        
    except Exception as e:
        print(f"Communication error: {e}")
        print("Tip: Make sure the Arduino sketch is uploaded and Bridge.provide('run_calc', ...) is in setup().")

if __name__ == "__main__":
    main()
    # App.run() is necessary to keep the communication channel open
    App.run()