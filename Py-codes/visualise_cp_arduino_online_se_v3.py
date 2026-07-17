import re
import csv as _csv
import numpy as np

# File path
file_path = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/online/online_cpt_t21_p3_ds1_rol.csv"

# Ideal baseline CP locations
IDEAL_CPS = [100, 300, 900, 1200, 1600]
# Tolerance window (detections within +/- 15 points of an ideal CP match it)
MATCH_TOLERANCE = 15 

# --------------------------------------------------------------------------
# Read and Parse Data
# --------------------------------------------------------------------------
with open(file_path, 'r') as f:
    all_lines = [line.rstrip('\n') for line in f.readlines()]

def parse_csv_line(line):
    return next(_csv.reader([line]))

data_lines = all_lines[1:] # Drop header

all_window_lengths = []
sequence_stats = {}

for i in range(0, len(data_lines) - 2, 3):
    cp_row  = parse_csv_line(data_lines[i])
    win_row = parse_csv_line(data_lines[i + 1])

    seq_name = cp_row[0].strip()
    if not seq_name:
        continue

    # 1. Parse detected CP values
    cp_vals = []
    for cell in cp_row[1:]:
        cell = cell.strip()
        if cell:
            try:
                cp_vals.append(int(float(cell)))
            except ValueError:
                pass

    # 2. Parse window strings and calculate (e - cp)
    e_vals = []
    for cell in win_row[1:]:
        cell = cell.strip().strip('"')
        m = re.match(r'\{(\d+),(\d+)\}', cell)
        if m:
            e_vals.append(int(m.group(2)))

    window_lengths = [e - cp for cp, e in zip(cp_vals, e_vals)]
    all_window_lengths.extend(window_lengths)

    # 3. Analyze Spurious / Extra CPs
    # Match detected CPs to ideal targets to filter out valid detections
    matched_indices = set()
    for ideal in IDEAL_CPS:
        closest_dist = float('inf')
        closest_idx = -1
        
        for idx, cp in enumerate(cp_vals):
            if idx in matched_indices:
                continue
            dist = abs(cp - ideal)
            if dist <= MATCH_TOLERANCE and dist < closest_dist:
                closest_dist = dist
                closest_idx = idx
                
        if closest_idx != -1:
            matched_indices.add(closest_idx)

    # Extra CPs are whatever detections remain unmatched
    extra_cps_count = len(cp_vals) - len(matched_indices)
    
    sequence_stats[seq_name] = {
        "total_detected": len(cp_vals),
        "extra_cps": extra_cps_count,
        "avg_window": np.mean(window_lengths) if window_lengths else 0
    }

# --------------------------------------------------------------------------
# Display Results
# --------------------------------------------------------------------------
print("=" * 50)
print("STATISTICAL ANALYSIS RESULTS")
print("=" * 50)

# 1. Average length of the bars
global_avg_window = np.mean(all_window_lengths) if all_window_lengths else 0
print(f"1. GLOBAL WINDOW STATS:")
print(f"   -> Average length of all bars (e - CP): {global_avg_window:.2f} points")
print(f"   -> Min window length: {min(all_window_lengths) if all_window_lengths else 0}")
print(f"   -> Max window length: {max(all_window_lengths) if all_window_lengths else 0}\n")

# 2. Extra CPs per sequence
print(f"2. EXTRA CPs DETECTED PER SEQUENCE (Ideal Baseline: {IDEAL_CPS}):")
print(f"   {'-' * 45}")
print(f"   {'Sequence':<10} | {'Total Found':<12} | {'Extra CPs':<10}")
print(f"   {'-' * 45}")

total_extra_cps = 0
for seq, stats in sorted(sequence_stats.items()):
    print(f"   {seq:<10} | {stats['total_detected']:<12} | {stats['extra_cps']:<10}")
    total_extra_cps += stats['extra_cps']

print(f"   {'-' * 45}")
print(f"   Total extra CPs across all data: {total_extra_cps}")
print("=" * 50)