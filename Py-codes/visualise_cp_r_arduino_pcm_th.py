import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# File paths from your environment
file1 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/detected_change_points_p3_t13_0cp.csv"
file2 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/cpt_100_2026_p3_t13_0cp.csv" 

# Read and Clean Data
# Using double-precision logic as established in your setup
df1 = pd.read_csv(file1).apply(pd.to_numeric, errors='coerce').dropna(how='all')
df2 = pd.read_csv(file2).apply(pd.to_numeric, errors='coerce').dropna(how='all')

# Align shapes to ensure consistent column and row indexing
min_rows = min(len(df1), len(df2))
min_cols = min(df1.shape[1], df2.shape[1])
df1 = df1.iloc[:min_rows, :min_cols]
df2 = df2.iloc[:min_rows, :min_cols]

# Plot Containers
arduino_x, arduino_y = [], []
r_x, r_y = [], []
mismatch_x, mismatch_y = [], []

# Analysis Loop for Change Point Detection (CPD) comparison
for col in range(min_cols):
    col_idx = col + 1
    col1 = df1.iloc[:, col] # Arduino Results
    col2 = df2.iloc[:, col] # R Reference Results
    
    # 1. Collect all points for the background scatter
    for val in col1.dropna():
        arduino_x.append(col_idx)
        arduino_y.append(val)
    for val in col2.dropna():
        r_x.append(col_idx)
        r_y.append(val)

    # 2. Pinpoint specific row-level mismatches
    for v1, v2 in zip(col1, col2):
        # Skip if they match perfectly (both NaN or same value)
        if (pd.isna(v1) and pd.isna(v2)) or (v1 == v2):
            continue
        
        # If there is a discrepancy, circle ONLY the values present in this specific row
        if not pd.isna(v1):
            mismatch_x.append(col_idx)
            mismatch_y.append(v1)
        if not pd.isna(v2):
            mismatch_x.append(col_idx)
            mismatch_y.append(v2)

# Plotting
plt.figure(figsize=(14, 7))

# Main Data Points: R in blue circles, Arduino in green crosses
plt.scatter(r_x, r_y, color='blue', label='R ', marker='o', alpha=0.5, s=30)
plt.scatter(arduino_x, arduino_y, color='green', label='Arduino', marker='x', alpha=0.8, s=40)

# Mismatch Indicators: Hollow red circles only on disagreement points
if mismatch_x:
    plt.scatter(mismatch_x, mismatch_y, facecolors='none', edgecolors='red', 
                s=120, label='Mismatch', linewidths=1.5)

# Formatting
plt.xlabel("Column Index")
plt.ylabel("Change Point Magnitude")
plt.title("CPD Comparison: R vs. Arduino") # Simplified title
plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
plt.grid(True, linestyle=':', alpha=0.6)
plt.xlim(0, min_cols + 1)
plt.tight_layout()

plt.show()

# Final Accuracy Summary for your MSc thesis report
match_count = (df1.values == df2.values).sum() + (pd.isna(df1.values) & pd.isna(df2.values)).sum()
total_vals = min_rows * min_cols
print(f"Total Values: {total_vals} | Matching: {match_count} | Accuracy: {(match_count/total_vals)*100:.2f}%")