import pandas as pd
import matplotlib.pyplot as plt

# File path
file_path = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/online/online_cpt_t12_p3_ds1_rol.csv"

# Read and Clean Data
# Coerce errors to NaN and drop completely empty rows
df1 = pd.read_csv(file_path).apply(pd.to_numeric, errors='coerce').dropna(how='all')

# Plot Containers
arduino_x, arduino_y = [], []
num_cols = df1.shape[1]

# Extraction Loop
print(f"Scanning {num_cols} columns for Changepoints...")
for col in range(num_cols):
    col_idx = col + 1
    
    # Get all valid points in this column, dropping NaNs[cite: 5]
    valid_points = df1.iloc[:, col].dropna()
    
    # Store points for the scatter plot[cite: 5]
    for val in valid_points:
        arduino_x.append(col_idx)
        arduino_y.append(val)

# Plotting
plt.figure(figsize=(14, 7))

# Main Data Points: Arduino in green crosses[cite: 5]
plt.scatter(arduino_x, arduino_y, color='green', label='Arduino', marker='x', alpha=0.8, s=40)

# Add horizontal marker lines
target_y_lines = [100, 300, 900, 1200, 1600]
for idx, y_val in enumerate(target_y_lines):
    # Only label the first line to avoid cluttering the legend
    label = 'Expected CPs' if idx == 0 else ""
    plt.axhline(y=y_val, color='red', linestyle='--', alpha=0.6, label=label)

# Formatting
plt.xlabel("Column Index")
plt.ylabel("Change Point Index")
plt.title("Arduino Online CPD Visualization") 
plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
plt.grid(True, linestyle=':', alpha=0.6)
plt.xlim(0, num_cols + 1)
plt.tight_layout()

plt.show()

print(f"Visualization complete. Plotted {len(arduino_x)} changepoints across {num_cols} columns.")