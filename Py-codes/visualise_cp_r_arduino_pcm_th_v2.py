import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# File paths
file1 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/detected_change_points_p10_t11_0cp.csv"
file2 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/cpt_100_2026_p10_t11_0cp.csv" 

# Read and Clean Data
df1 = pd.read_csv(file1).apply(pd.to_numeric, errors='coerce').dropna(how='all')
df2 = pd.read_csv(file2).apply(pd.to_numeric, errors='coerce').dropna(how='all')

# Align shapes [cite: 1, 13]
min_cols = min(df1.shape[1], df2.shape[1])
df1 = df1.iloc[:, :min_cols]
df2 = df2.iloc[:, :min_cols]

# Plot Containers
arduino_x, arduino_y = [], []
r_x, r_y = [], []
mismatch_x, mismatch_y = [], []
discrepancy_cols = 0

# Analysis Loop
print("Scanning for discrepancies...")
for col in range(min_cols):
    col_idx = col + 1
    # Get all valid points in this column as sets
    set_arduino = set(df1.iloc[:, col].dropna())
    set_r = set(df2.iloc[:, col].dropna())
    
    # 1. Store all points for the primary scatter plot
    for val in set_arduino:
        arduino_x.append(col_idx)
        arduino_y.append(val)
    for val in set_r:
        r_x.append(col_idx)
        r_y.append(val)

    # 2. Set-Based Post-Processing: Find points unique to either method
    unique_to_arduino = set_arduino - set_r
    unique_to_r = set_r - set_arduino
    
    if unique_to_arduino or unique_to_r:
        discrepancy_cols += 1
        
        # --- NEW PRINT STATEMENTS ---
        print(f"\nDiscrepancy in Column {col_idx}:")
        if unique_to_arduino:
            print(f"  -> Arduino detected {sorted(list(unique_to_arduino))} which R missed.")
        if unique_to_r:
            print(f"  -> R detected {sorted(list(unique_to_r))} which Arduino missed.")
        # ----------------------------

        # Only add circles to points that don't have a partner in the other set
        for val in unique_to_arduino:
            mismatch_x.append(col_idx)
            mismatch_y.append(val)
        for val in unique_to_r:
            mismatch_x.append(col_idx)
            mismatch_y.append(val)

# Plotting
plt.figure(figsize=(14, 7))

# 1. Main Data Points: R in blue circles, Arduino in green crosses
plt.scatter(r_x, r_y, color='blue', label='R', marker='o', alpha=0.4, s=30)
plt.scatter(arduino_x, arduino_y, color='green', label='Arduino', marker='x', alpha=0.8, s=40)

# 2. Mismatch Indicators: Red hollow circles ONLY for set differences
if mismatch_x:
    plt.scatter(mismatch_x, mismatch_y, facecolors='none', edgecolors='red', 
                s=120, label='Mismatch', linewidths=1.5)

# Formatting
plt.xlabel("Column Index")
plt.ylabel("Change Point Magnitude")
plt.title("CPD Comparison") 
plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
plt.grid(True, linestyle=':', alpha=0.6)
plt.xlim(0, min_cols + 1)
plt.tight_layout()

plt.show()

print(f"Post-processing complete. Found {len(mismatch_x)} specific point discrepancies across {discrepancy_cols} files.")