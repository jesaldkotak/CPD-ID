import csv
import os
import matplotlib.pyplot as plt

def load_csv_data(filename, value_col_index):
    if not os.path.exists(filename):
        print(f"Error: {filename} not found.")
        return []

    data = []
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) > value_col_index:
                try:
                    data.append(float(row[value_col_index]))
                except ValueError:
                    continue
    return data

def main():
    arduino_file = "C:/Users/Jesal Kotak/OneDrive - Imperial College London/ADIC MSc/Thesis Project/R-Codes/arduino_cusum_results.csv"
    r_file = "C:/Users/Jesal Kotak/OneDrive - Imperial College London/ADIC MSc/Thesis Project/R-Codes/r_cusum_results.csv"  # Ensure your R script outputs to this name!
    output_file = "cusum_comparison.csv"

    print("Loading datasets...")
    arduino_data = load_csv_data(arduino_file, 1)
    r_data = load_csv_data(r_file, 1)

    if not arduino_data or not r_data:
        print("Data missing. Check filenames.")
        return

    min_len = min(len(arduino_data), len(r_data))
    
    # Calculate stats and differences
    indices = list(range(1, min_len + 1))
    ard_vals = arduino_data[:min_len]
    r_vals = r_data[:min_len]
    diffs = [a - r for a, r in zip(ard_vals, r_vals)]
    abs_diffs = [abs(d) for d in diffs]

    # Save to CSV
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Index", "Arduino_Value", "R_Value", "Difference", "Abs_Difference"])
        for i in range(min_len):
            writer.writerow([indices[i], repr(ard_vals[i]), repr(r_vals[i]), repr(diffs[i]), repr(abs_diffs[i])])

    print(f"CSV saved to {output_file}. Generating plots...")

    # --- PLOTTING LOGIC ---
    
    # Figure 1: Overlay (Square Aspect Ratio)
    plt.figure(figsize=(6, 6)) 
    plt.plot(indices, r_vals, label='R Implementation (Double)', color='blue', alpha=0.6, linewidth=2)
    plt.plot(indices, ard_vals, label='Arduino Implementation (Float)', color='red', linestyle='--', alpha=0.8)
    plt.ylabel('CUSUM Statistic Value')
    plt.xlabel('Index (Time Point)')
    plt.title('CUSUM Comparison: R vs. Arduino MCU')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()

    # Figure 2: Difference (Square Aspect Ratio)
    plt.figure(figsize=(6, 6))
    plt.plot(indices, diffs, color='black')
    plt.ylabel('Difference')
    plt.xlabel('Index')
    #plt.title('Deviation')
    # plt.legend()
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()

    # This will now open two separate windows
    plt.show()

    print(f"Max Absolute Difference: {max(abs_diffs)}")

if __name__ == "__main__":
    main()


