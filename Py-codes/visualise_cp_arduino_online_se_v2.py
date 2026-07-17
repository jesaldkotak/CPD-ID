import matplotlib.pyplot as plt
import re
import csv as _csv

# File path
file_path = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/online/online_cpt_t15_p3_ds1_rol.csv"

# --------------------------------------------------------------------------
# Read and parse the CSV format
# --------------------------------------------------------------------------
with open(file_path, 'r') as f:
    all_lines = [line.rstrip('\n') for line in f.readlines()]

def parse_csv_line(line):
    """Split a CSV line, respecting quoted fields (e.g. \"{1,300}\")."""
    return next(_csv.reader([line]))

# Skip the header line, then process data lines in groups of 3
data_lines = all_lines[1:]   

# Dictionary to hold the parsed window lengths for each sequence
# Format: { 'x1': [len1, len2, ...], 'x2': [...] }
sequence_window_lengths = {}

for i in range(0, len(data_lines) - 2, 3):
    cp_row  = parse_csv_line(data_lines[i])#[cite: 6]
    win_row = parse_csv_line(data_lines[i + 1])#[cite: 6]

    seq_name = cp_row[0].strip()#[cite: 6]
    if not seq_name:
        continue

    # Parse CP values
    cp_vals = []
    for cell in cp_row[1:]:
        cell = cell.strip()
        if cell:
            try:
                cp_vals.append(int(float(cell)))
            except ValueError:
                pass

    # Parse window strings — format is "{s,e}"[cite: 6]
    e_vals = []
    for cell in win_row[1:]:
        cell = cell.strip().strip('"')
        m = re.match(r'\{(\d+),(\d+)\}', cell)
        if m:
            e_vals.append(int(m.group(2)))   # we only need e here

    # Calculate (e - cp) for each pair and store them
    window_lengths = []
    for cp, e in zip(cp_vals, e_vals):
        window_lengths.append(e - cp)
        
    if window_lengths:
        sequence_window_lengths[seq_name] = window_lengths

# --------------------------------------------------------------------------
# Plotting the Window Lengths
# --------------------------------------------------------------------------
plt.figure(figsize=(14, 7))

# Get a color palette to differentiate CP ranks (CP_1, CP_2, etc.)
colors = plt.cm.tab10.colors  

current_x = 0
tick_positions = []
tick_labels = []
labeled_cp_ranks = set()

# Iterate through each sequence and place bars side-by-side
for seq_name, lengths in sequence_window_lengths.items():
    n_bars = len(lengths)
    group_positions = []
    
    for idx, length in enumerate(lengths):
        bar_pos = current_x + idx
        group_positions.append(bar_pos)
        
        # Track labels so we don't repeat "CP 1", "CP 2" down in the legend
        cp_rank_label = f"CP {idx + 1}"
        if cp_rank_label not in labeled_cp_ranks:
            label = cp_rank_label
            labeled_cp_ranks.add(cp_rank_label)
        else:
            label = ""
            
        plt.bar(bar_pos, length, color=colors[idx % 10], edgecolor='black', 
                width=0.8, alpha=0.85, label=label)
        
    # Calculate center of the current cluster for the X-axis label
    if group_positions:
        center_x = sum(group_positions) / n_bars
        tick_positions.append(center_x)
        tick_labels.append(seq_name)
        
    # Advance the X index past the group and add a padding gap of 3 units
    current_x += n_bars + 3

plt.axhline(y=3, color='green', linestyle='--', alpha=0.7, linewidth=1.5, label='points')

# Formatting
plt.xlabel("Sequence Index", fontsize=12, fontweight='bold')
plt.ylabel("Window Length (e - CP)", fontsize=12, fontweight='bold')
plt.title("Detection Window Lengths per Change Point", fontsize=14, fontweight='bold')

plt.xticks(tick_positions, tick_labels)
plt.grid(True, axis='y', linestyle=':', alpha=0.6)
plt.legend(title="CP Rank", loc='upper right', bbox_to_anchor=(1.12, 1))
plt.tight_layout()

plt.show()