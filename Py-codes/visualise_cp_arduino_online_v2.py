import matplotlib.pyplot as plt
import re
import csv as _csv

# File path
file_path = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/online/online_cpt_t18_p3_ds1_rol.csv"

# --------------------------------------------------------------------------
# Read and parse the new CSV format.
#
# Each data sequence occupies exactly 3 consecutive rows after the header:
#   Row 1 (cp)      : sequence_name, cp1, cp2, ...
#   Row 2 (window)  : sequence_name, {s1,e1}, {s2,e2}, ...
#   Row 3 (latency) : sequence_name, total_time, avg_rtt, avg_exec_us
#
# Row widths are irregular (latency row may have more or fewer fields than
# the CP row), so we parse raw lines directly instead of using pandas.
# --------------------------------------------------------------------------
with open(file_path, 'r') as f:
    all_lines = [line.rstrip('\n') for line in f.readlines()]

def parse_csv_line(line):
    """Split a CSV line, respecting quoted fields (e.g. \"{1,300}\")."""
    return next(_csv.reader([line]))

# Plot containers
cp_x,  cp_y  = [], []   # scatter: (col_index, cp_value)
seg_x,  seg_y0, seg_y1 = [], [], []   # vlines: x, cp_value, e_value

num_sequences = 0

# Skip the header line, then process data lines in groups of 3
data_lines = all_lines[1:]   # drop header

for i in range(0, len(data_lines) - 2, 3):
    cp_row  = parse_csv_line(data_lines[i])
    win_row = parse_csv_line(data_lines[i + 1])
    # data_lines[i + 2] is the latency row — not needed for plotting

    seq_name = cp_row[0].strip()

    # Extract numeric sequence index from name (e.g. "x1" -> 1, "x42" -> 42)
    match = re.search(r'\d+', seq_name)
    if not match:
        continue
    col_idx = int(match.group())
    num_sequences += 1

    # Parse CP values from cp_row (columns 1 onwards, skip empty)
    cp_vals = []
    for cell in cp_row[1:]:
        cell = cell.strip()
        if cell:
            try:
                cp_vals.append(int(float(cell)))
            except ValueError:
                pass

    # Parse window strings from win_row — format is "{s,e}"
    e_vals = []
    for cell in win_row[1:]:
        cell = cell.strip().strip('"')
        m = re.match(r'\{(\d+),(\d+)\}', cell)
        if m:
            e_vals.append(int(m.group(2)))   # we only need e here

    # Pair each CP with its e value; zip stops at the shorter list
    for cp, e in zip(cp_vals, e_vals):
        cp_x.append(col_idx)
        cp_y.append(cp)
        seg_x.append(col_idx)
        seg_y0.append(cp)
        seg_y1.append(e)

# --------------------------------------------------------------------------
# Plot
# --------------------------------------------------------------------------
plt.figure(figsize=(14, 7))

# Vertical line segments from cp up to e for each detection (Detection window in RED)
# Only plots if the window size (y1 - y0) is greater than 10 points
plotted_windows = 0
for x, y0, y1 in zip(seg_x, seg_y0, seg_y1):
    if (y1 - y0) > 10:
        plt.plot([x, x], [y0, y1], color='red', alpha=1, linewidth=4.5)
        plotted_windows += 1

# CP scatter points: BLACK crosses
plt.scatter(cp_x, cp_y, color='black', alpha = 0.5, label='Detected CP', marker='x', s=50, zorder=3)

# Dummy line handle for the legend entry for the window lines
plt.plot([], [], color='red', alpha=1, linewidth=4.5, label='Detection window')

# Expected CP reference lines: GREEN dashed lines
target_y_lines = [100, 300, 900, 1200, 1600]
for idx, y_val in enumerate(target_y_lines):
    label = 'Expected CPs' if idx == 0 else ""
    plt.axhline(y=y_val, color='green', linestyle='--', alpha=0.7, linewidth=1.5, label=label)

# Formatting
plt.xlabel("Sequence Index")
plt.ylabel("Change Point / Window End Index")
plt.title("Arduino Online CPD Visualisation")
plt.legend(loc='upper right', bbox_to_anchor=(1.2, 1))
plt.grid(True, linestyle=':', alpha=0.6)
plt.xlim(0, num_sequences + 1)
plt.tight_layout()

plt.show()

print(f"Visualisation complete.")
print(f"Total changepoints found: {len(cp_x)} across {num_sequences} sequences.")
print(f"Plotted {plotted_windows} windows that exceeded the 10-point threshold.")