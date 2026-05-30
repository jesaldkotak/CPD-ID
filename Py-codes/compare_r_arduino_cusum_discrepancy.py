import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# --- Configuration ---
file_r = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/Cusum_values/ard_csm_val_2026_p10_t11_0cp_30.csv"
file_arduino = "C:/Users/jesal/OneDrive - Imperial College London/ADIC MSc/Thesis Project/Programming/Results/Datafiles/Cusum_values/r_csm_val_2026_p10_t11_0cp_30.csv" 
output_file = "cusum_discrepancies_76.csv"

def main():
    print(f"Reading {file_r} and {file_arduino}...")
    
    # Read data natively
    df_r = pd.read_csv(file_r)
    df_ard = pd.read_csv(file_arduino)

    # Coerce only the specific math columns just to be safe
    df_r['Max_CUSUM'] = pd.to_numeric(df_r['Max_CUSUM'], errors='coerce')
    df_ard['Max_CUSUM'] = pd.to_numeric(df_ard['Max_CUSUM'], errors='coerce')

    # Because a precision difference might cause one algorithm to detect a CP early 
    # and change the total number of iterations, we strictly align to the shortest file.
    min_len = min(len(df_r), len(df_ard))
    df_r = df_r.iloc[:min_len].reset_index(drop=True)
    df_ard = df_ard.iloc[:min_len].reset_index(drop=True)
    
    if min_len == 0:
        print("Error: One or both files are empty.")
        return

    # Create a combined comparison dataframe
    df_diff = pd.DataFrame({
        'Step': range(1, min_len + 1),
        'Direction': df_r['Direction'],
        'Exp_Interval': df_r['Exp_Interval'],
        'Candidate_Index_R': df_r['Candidate_Index'],
        'Candidate_Index_Ard': df_ard['Candidate_Index'],
        'Max_CUSUM_R': df_r['Max_CUSUM'],
        'Max_CUSUM_Ard': df_ard['Max_CUSUM']
    })

    # Calculate the raw difference and absolute magnitude
    df_diff['Difference'] = df_diff['Max_CUSUM_R'] - df_diff['Max_CUSUM_Ard']
    df_diff['Abs_Magnitude'] = df_diff['Difference'].abs()

    # Save to a new CSV for detailed inspection
    df_diff.to_csv(output_file, index=False)
    print(f"Successfully saved {min_len} comparison rows to {output_file}")

    # --- Plotting ---
    plt.figure(figsize=(12, 6))

    # We use a slight offset (1e-12) so zero-difference points don't break the log scale
    plt.plot(df_diff['Step'], df_diff['Abs_Magnitude'] + 1e-12, 
             marker='o', linestyle='-', color='red', alpha=0.7, markersize=4, label='Discrepancy Magnitude')

    # # --- NEW VERTICAL LINES CODE ---
    # v_lines = [68, 70, 72, 78, 80, 80, 82, 84, 343, 345]
    # for x_val in v_lines:
    #     # Drawing dashed blue vertical lines 
    #     plt.axvline(x=x_val, color='blue', linestyle='--', alpha=0.5)
    # # -------------------------------

    plt.xlabel("Iteration Step (Right-odd Left-even)")
    plt.ylabel("Absolute Discrepancy")
    #plt.title("Tracking CUSUM Floating-Point Drift")
    
    # Use a logarithmic scale to see the tiny decimal errors grow
    plt.yscale('log')
    plt.grid(True, which="both", ls=":", alpha=0.6)
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()