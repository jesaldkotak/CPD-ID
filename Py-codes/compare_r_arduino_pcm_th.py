import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# File names

file1 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/detected_change_points_0cp_t11.csv"
file2 = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/0cp/cpt_100_2026_0cp_t11.csv" 



# Read CSVs
df1 = pd.read_csv(file1)
df2 = pd.read_csv(file2)

# Convert everything to numeric (NA/strings -> NaN)
df1 = df1.apply(pd.to_numeric, errors='coerce')
df2 = df2.apply(pd.to_numeric, errors='coerce')

# Drop rows that are completely NaN (handles your extra NA row)
df1 = df1.dropna(how='all')
df2 = df2.dropna(how='all')

# Align shapes (in case one still has extra rows)
min_rows = min(len(df1), len(df2))
df1 = df1.iloc[:min_rows, :]
df2 = df2.iloc[:min_rows, :]

# Align number of columns
min_cols = min(df1.shape[1], df2.shape[1])
df1 = df1.iloc[:, :min_cols]
df2 = df2.iloc[:, :min_cols]

total_values = min_rows * min_cols
match_count = 0

print("\n Comparing files column-wise...\n")

for col in range(min_cols):
    col1 = df1.iloc[:, col]
    col2 = df2.iloc[:, col]

    mismatches = []

    for i, (v1, v2) in enumerate(zip(col1, col2)):
        if pd.isna(v1) and pd.isna(v2):
            match_count += 1
        elif v1 == v2:
            match_count += 1
        else:
            mismatches.append((i, v1, v2))

    if mismatches:
        print(f"Column {col+1} mismatch:")
        for row, v1, v2 in mismatches:
            print(f"   Row {row}: File1={v1}, File2={v2}")
        print()



mismatch_x = []   # column index
mismatch_y = []   # mismatch value

for col in range(min_cols):
    col1 = df1.iloc[:, col]
    col2 = df2.iloc[:, col]

    for v1, v2 in zip(col1, col2):
        if pd.isna(v1) and pd.isna(v2):
            continue
        elif v1 == v2:
            continue
        else:
            mismatch_x.append(col + 1)          # column number (1-based)
            mismatch_y.append(v1 - v2)          # difference

# Plot
plt.figure()
plt.scatter(mismatch_x, mismatch_y)
plt.xlabel("Column Index")
plt.ylabel("Arduino - R")
# plt.title("Column-wise Mismatch Scatter")
plt.grid()
plt.axhline(y=0, color='k')
# plt.axvline(x=0, color='k')
plt.show()

# Accuracy
accuracy = (match_count / total_values) * 100

print(f"Accuracy: {accuracy:.2f}% ({match_count}/{total_values} values match)")