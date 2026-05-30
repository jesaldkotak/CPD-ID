import pandas as pd
import matplotlib.pyplot as plt
import os

def main():
    input_file = "cusum_comparison.csv"

    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found.")
        return

    # Load data as strings to handle potential formatting/quote issues
    df = pd.read_csv(input_file, dtype=str)

    # Function to strip potential quotes and convert to float
    def clean_to_float(column):
        return pd.to_numeric(column.str.replace("'", "").str.strip(), errors='coerce')

    # Assign x (1st column) and y (4th column)
    x = clean_to_float(df.iloc[:, 0])
    y = clean_to_float(df.iloc[:, 3])

    # --- Plotting ---
    plt.figure(figsize=(6, 6)) # Maintains square aspect ratio
    
    plt.plot(x, y, color='purple', linewidth=1, label='Error/Difference')
    
    # Restoring specific legends and labels
    plt.title('Deviation Analysis (4th Column)')
    plt.xlabel('Index (1st Column)')
    plt.ylabel('Difference Value')
    
    # Ensure the Y-axis uses scientific notation (e.g., 1e-5 or 1e-7)
    plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
    
    plt.legend(loc='upper left')
    plt.grid(True, linestyle='--', alpha=0.5)
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()