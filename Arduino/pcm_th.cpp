#include "pcm_th.h"
#include "s_e_points.h"
#include <math.h>

// Updated to use the stable, double-precision formula
static void calculate_cusum_local(double x[], int length, double res[]) {
    if (length < 2) return;
    
    double total_sum = 0.0;
    double current_y = 0.0;
    
    for (int i = 0; i < length; i++) {
        total_sum += x[i];
    }

    double fn = (double)length;
    
    for (int i = 1; i < length; i++) {
        current_y += x[i-1]; 
        double fi = (double)i;
        
        // Simplified, numerically stable formula
        double numerator = (current_y * fn) - (fi * total_sum);
        double denominator = sqrt(fn * fi * (fn - fi));
        
        res[i-1] = numerator / denominator;
    }
}

static void sort_double(double arr[], int n) {
    for(int i=0; i<n-1; i++) {
        for(int j=0; j<n-i-1; j++) {
            if(arr[j] > arr[j+1]) { double t=arr[j]; arr[j]=arr[j+1]; arr[j+1]=t; }
        }
    }
}

static double median(double arr[], int n) {
    double* temp = new double[n];
    for(int i=0; i<n; i++) temp[i] = arr[i];
    sort_double(temp, n);
    double res;
    if (n % 2 == 0) res = (temp[n / 2 - 1] + temp[n / 2]) / 2.0;
    else res = temp[n / 2];
    delete[] temp;
    return res;
}

static double estimate_sigma(double x[], int n) {
    if (n < 2) return 0.0;
    double* diffs = new double[n - 1];
    for (int i = 1; i < n; ++i) diffs[i - 1] = (x[i] - x[i - 1]) / sqrt(2.0);
    double med = median(diffs, n - 1);
    
    double* abs_devs = new double[n - 1];
    for (int i = 0; i < n - 1; ++i) abs_devs[i] = fabs(diffs[i] - med);
    double sigma = 1.4826 * median(abs_devs, n - 1);
    
    delete[] diffs;
    delete[] abs_devs;
    return sigma;
}

// Ensure your pcm_th.h header is also updated to accept `double x[]` instead of `float`
void pcm_th(double x[], int length_x, double thr_const, int s, int e, int points, int k_l, int k_r, int cpt_out[], int& cpt_count) {
    
    double sigma = estimate_sigma(x, length_x);
    double thr_fin = thr_const * sigma * sqrt(2.0 * log((double)length_x));

    if (thr_fin <= 1e-5) thr_fin = 1e-5;

    if (e - s <= 1) return;
    
    int num_seq = length_x / points;
    if (num_seq <= 0) return;

    int* r_e_points = new int[num_seq];
    int* l_e_points = new int[num_seq];

    for (int i = 0; i < num_seq; i++){
        r_e_points[i] = (i+1) * points;
        l_e_points[i] = length_x - points + 1 - i * points;
    }

    int chp = 0; 
    int* right_points = new int[num_seq + 2];
    int* left_points = new int[num_seq + 2];
    int rur = 0, lur = 0;
    
    s_e_points(r_e_points, l_e_points, s, e, num_seq, right_points, rur, left_points, lur);

    delete[] r_e_points;
    delete[] l_e_points;

    if (k_r < k_l) {
        while ((chp == 0) && k_r < k_l && k_r <= rur) {
            int end_idx = right_points[k_r - 1];
            int sub_len = end_idx - s + 1;
            
            if (sub_len >= 2) {
                double* x_temp_r = new double[sub_len];
                double* ipcr = new double[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_r[i] = x[s + i - 1];
                
                calculate_cusum_local(x_temp_r, sub_len, ipcr);

                double ipcr_max = -1.0;
                int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabs(ipcr[i]);
                        arg_max_ipcr = i;
                    }
                }
                if(ipcr_max > thr_fin) chp = arg_max_ipcr + s;
                else k_r++;
                
                delete[] x_temp_r;
                delete[] ipcr;
            } else { k_r++; }
        }
    }

    if (k_l < k_r) {
        while ((chp == 0) && k_l < k_r && k_l <= lur) {
            int start_idx = left_points[k_l - 1];
            int sub_len = e - start_idx + 1;
            
            if (sub_len >= 2) {
                double* x_temp_l = new double[sub_len];
                double* ipcl = new double[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_l[i] = x[start_idx + i - 1];
                
                calculate_cusum_local(x_temp_l, sub_len, ipcl);

                double ipcl_max = -1.0;
                int arg_max_ipcl = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcl[i]) > ipcl_max) {
                        ipcl_max = fabs(ipcl[i]);
                        arg_max_ipcl = i;
                    }
                }
                if (ipcl_max > thr_fin) chp = arg_max_ipcl + start_idx;
                else k_l++;
                
                delete[] x_temp_l;
                delete[] ipcl;
            } else { k_l++; }
        }
    }

    if (chp == 0) {
        while ((chp == 0) && k_l <= lur && k_r <= rur) {
            int end_idx = right_points[k_r - 1];
            int sub_len = end_idx - s + 1;
            
            if (sub_len >= 2) {
                double* x_temp_r = new double[sub_len];
                double* ipcr = new double[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_r[i] = x[s + i - 1];

                calculate_cusum_local(x_temp_r, sub_len, ipcr);

                double ipcr_max = -1.0;
                int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabs(ipcr[i]); arg_max_ipcr = i;
                    }
                }
                
                if (ipcr_max > thr_fin) {
                    chp = arg_max_ipcr + s;
                } else {
                    int start_idx = left_points[k_l - 1];
                    int sub_len_l = e - start_idx + 1;
                    
                    if (sub_len_l >= 2) {
                        double* x_temp_l = new double[sub_len_l];
                        double* ipcl = new double[sub_len_l];
                        for (int i = 0; i < sub_len_l; i++) x_temp_l[i] = x[start_idx + i - 1];
                        
                        calculate_cusum_local(x_temp_l, sub_len_l, ipcl);

                        double ipcl_max = -1.0;
                        int arg_max_ipcl_l = 0;
                        for (int i = 0; i < sub_len_l - 1; i++) {
                            if (fabs(ipcl[i]) > ipcl_max) {
                                ipcl_max = fabs(ipcl[i]); arg_max_ipcl_l = i;
                            }
                        }

                        if (ipcl_max > thr_fin) chp = arg_max_ipcl_l + start_idx;
                        else { k_r++; k_l++; }
                        
                        delete[] x_temp_l;
                        delete[] ipcl;
                    } else { k_r++; k_l++; }
                }
                delete[] x_temp_r;
                delete[] ipcr;
            } else { k_r++; k_l++; }
        }
    }
    
    delete[] right_points;
    delete[] left_points;
    
    // RECURSION
    if (chp != 0) {
        bool exists = false;
        for(int i = 0; i < cpt_count; i++) {
            if(cpt_out[i] == chp) exists = true;
        }
        
        if(!exists && cpt_count < 100) {
            cpt_out[cpt_count++] = chp;
        }
    
        if ((chp - 1) - s >= points) {
            pcm_th(x, length_x, thr_const, s, chp - 1, points, 1, k_r, cpt_out, cpt_count);
        }
    
        if (e - (chp + 1) >= points) {
            int next_kl = (k_l - 1 > 1) ? k_l - 1 : 1;
            pcm_th(x, length_x, thr_const, chp + 1, e, points, next_kl, 1, cpt_out, cpt_count);
        }
    }
}
