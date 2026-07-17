#include "pcm_th.h"
#include "s_e_points.h"
#include <math.h>

extern double debug_buffer[]; // Changed to double
extern int debug_count;
#define MAX_DEBUG_LOGS 1000

static void calculate_cumsum_local(double x[], int length, double res[]) {
    res[0] = 0.0;
    if (length <= 2) return;
    
    double* a = new double[length-2];
    double* be = new double[length-2];
    double* y1 = new double[length];
    double* y = new double[length];
    
    y1[0] = x[0];
    y[0] = x[0];
    for (int i = 1; i < length; i++) y1[i] = y1[i-1] + x[i]*(double)(i+1);
    for (int i = 1; i < length; i++) y[i] = y[i-1] + x[i];
    
    double fn = (double)length;  
    for (int i = 0; i < length - 2; i++){
        double fi = (double)(i+2);
        a[i] = sqrt(6/((fn - 1) * fn * (fn + 1) * (2 - 2 * fi* fi + 2 * fi * fn - 1 + 2 * fi - fn)));
        be[i] = sqrt(((fn - fi + 1) * (fn - fi))/((fi - 1) * fi));
    }
    
    res[0] = 0;
    for (int i = 0; i < length-2; i++) {
        double fi = (double)(i+2);
        res[i+1] = a[i] * be[i] * ((2 * fi + fn - 1) * y1[i+1] - (fn + 1) * fi * y[i+1]) - (a[i]/be[i]) * ((3 * fn - 2 * fi + 1) * (y1[length-1] -  y1[i+1]) - (fn + 1) * (2 * fn - fi) * (y[length-1] - y[i+1]));
    }
    
    delete[] y; delete[] y1; delete[] a; delete[] be;
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
    if (n < 3) return 0.0;
    double* diffs = new double[n - 1];
    double* der = new double[n - 2];
    
    for (int i = 1; i < n; ++i) diffs[i - 1] = (x[i] - x[i - 1]);
    for (int i = 1; i < n-1; ++i) der[i - 1] = (diffs[i] - diffs[i - 1])/ sqrt(6.0);
  
    double med = median(der, n - 2);
    
    double* abs_devs = new double[n - 2];
    for (int i = 0; i < n - 2; ++i) abs_devs[i] = fabs(der[i] - med);
    double sigma = 1.4826 * median(abs_devs, n - 2);
    
    delete[] diffs; delete[] der; delete[] abs_devs;
    return sigma;
}

void pcm_th(double x[], int length_x, double thr_const, int s, int e, int points, int k_l, int k_r, int cpt_out[], int& cpt_count, int& cp_order) {
    
    double sigma = estimate_sigma(x, length_x);
    double thr_fin = thr_const * sigma * sqrt(2.0 * log((double)length_x));

    if (thr_fin <= 1e-5) thr_fin = 1e-5;

    if (debug_count == 0) {
        int base = debug_count * 9;
        debug_buffer[base + 0] = 0.0; 
        debug_buffer[base + 1] = 0.0;
        debug_buffer[base + 2] = 0.0;
        debug_buffer[base + 3] = 0.0;
        debug_buffer[base + 4] = 0.0;
        debug_buffer[base + 5] = thr_fin; 
        debug_buffer[base + 6] = 0.0;
        debug_buffer[base + 7] = 0.0;
        debug_buffer[base + 8] = 0.0;
        debug_count++;
    }
  
    if (e - s <= 2) return;
    
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

    delete[] r_e_points; delete[] l_e_points;

    if (k_r < k_l) {
        while ((chp == 0) && k_r < k_l && k_r < rur) {
            int end_idx = right_points[k_r - 1];
            int sub_len = end_idx - s + 1;
            if (sub_len >= 2) {
                double* x_temp_r = new double[sub_len];
                double* ipcr = new double[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_r[i] = x[s + i - 1];
                calculate_cumsum_local(x_temp_r, sub_len, ipcr);

                double ipcr_max = -1.0; int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabs(ipcr[i]); arg_max_ipcr = i;
                    }
                }
            
                if(ipcr_max > thr_fin) {
                    chp = arg_max_ipcr + s;
                    cp_order++; 
                }

                double ipcr_before = (arg_max_ipcr > 0) ? fabs(ipcr[arg_max_ipcr - 1]) : 0.0;
                double ipcr_after = (arg_max_ipcr < sub_len - 2) ? fabs(ipcr[arg_max_ipcr + 1]) : 0.0;

                if (debug_count < MAX_DEBUG_LOGS) {
                    int base = debug_count * 9;
                    debug_buffer[base + 0] = 1.0; 
                    debug_buffer[base + 1] = (double)k_r;
                    debug_buffer[base + 2] = (double)s; 
                    debug_buffer[base + 3] = (double)end_idx; 
                    debug_buffer[base + 4] = (double)(arg_max_ipcr + s);
                    debug_buffer[base + 5] = ipcr_max;
                    debug_buffer[base + 6] = (double)cp_order;
                    debug_buffer[base + 7] = ipcr_before;
                    debug_buffer[base + 8] = ipcr_after;
                    debug_count++;
                }
                if(ipcr_max <= thr_fin) k_r++;
                
                delete[] x_temp_r; delete[] ipcr;
            } else { k_r++; }
        }
    }

    if (k_l < k_r) {
        while ((chp == 0) && k_l < k_r && k_l < lur) {
            int start_idx = left_points[k_l - 1];
            int sub_len = e - start_idx + 1;
            if (sub_len >= 2) {
                double* x_temp_l = new double[sub_len];
                double* ipcl = new double[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_l[i] = x[start_idx + i - 1];
                calculate_cumsum_local(x_temp_l, sub_len, ipcl);

                double ipcl_max = -1.0; int arg_max_ipcl = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcl[i]) > ipcl_max) {
                        ipcl_max = fabs(ipcl[i]); arg_max_ipcl = i;
                    }
                }

                if (ipcl_max > thr_fin) {
                    chp = arg_max_ipcl + start_idx;
                    cp_order++;
                }

                double ipcl_before = (arg_max_ipcl > 0) ? fabs(ipcl[arg_max_ipcl - 1]) : 0.0;
                double ipcl_after = (arg_max_ipcl < sub_len - 2) ? fabs(ipcl[arg_max_ipcl + 1]) : 0.0;

                if (debug_count < MAX_DEBUG_LOGS) {
                    int base = debug_count * 9;
                    debug_buffer[base + 0] = -1.0; 
                    debug_buffer[base + 1] = (double)k_l;
                    debug_buffer[base + 2] = (double)start_idx; 
                    debug_buffer[base + 3] = (double)e;         
                    debug_buffer[base + 4] = (double)(arg_max_ipcl + start_idx);
                    debug_buffer[base + 5] = ipcl_max;
                    debug_buffer[base + 6] = (double)cp_order;
                    debug_buffer[base + 7] = ipcl_before;
                    debug_buffer[base + 8] = ipcl_after;
                    debug_count++;
                }
                if (ipcl_max <= thr_fin) k_l++;
                
                delete[] x_temp_l; delete[] ipcl;
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
                calculate_cumsum_local(x_temp_r, sub_len, ipcr);

                double ipcr_max = -1.0; int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabs(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabs(ipcr[i]); arg_max_ipcr = i;
                    }
                }

                if (ipcr_max > thr_fin) {
                    chp = arg_max_ipcr + s;
                    cp_order++;
                }

                double ipcr_before = (arg_max_ipcr > 0) ? fabs(ipcr[arg_max_ipcr - 1]) : 0.0;
                double ipcr_after = (arg_max_ipcr < sub_len - 2) ? fabs(ipcr[arg_max_ipcr + 1]) : 0.0;

                if (debug_count < MAX_DEBUG_LOGS) {
                    int base = debug_count * 9;
                    debug_buffer[base + 0] = 1.0; 
                    debug_buffer[base + 1] = (double)k_r;
                    debug_buffer[base + 2] = (double)s;
                    debug_buffer[base + 3] = (double)end_idx;
                    debug_buffer[base + 4] = (double)(arg_max_ipcr + s);
                    debug_buffer[base + 5] = ipcr_max;
                    debug_buffer[base + 6] = (double)cp_order;
                    debug_buffer[base + 7] = ipcr_before;
                    debug_buffer[base + 8] = ipcr_after;
                    debug_count++;
                }

                if (ipcr_max <= thr_fin) {
                    int start_idx = left_points[k_l - 1];
                    int sub_len_l = e - start_idx + 1;
                    
                    if (sub_len_l >= 2) {
                        double* x_temp_l = new double[sub_len_l];
                        double* ipcl = new double[sub_len_l];
                        for (int i = 0; i < sub_len_l; i++) x_temp_l[i] = x[start_idx + i - 1];
                        calculate_cumsum_local(x_temp_l, sub_len_l, ipcl);

                        double ipcl_max = -1.0; int arg_max_ipcl_l = 0;
                        for (int i = 0; i < sub_len_l - 1; i++) {
                            if (fabs(ipcl[i]) > ipcl_max) {
                                ipcl_max = fabs(ipcl[i]); arg_max_ipcl_l = i;
                            }
                        }

                        if (ipcl_max > thr_fin) {
                            chp = arg_max_ipcl_l + start_idx;
                            cp_order++;
                        }

                        double ipcl_before = (arg_max_ipcl_l > 0) ? fabs(ipcl[arg_max_ipcl_l - 1]) : 0.0;
                        double ipcl_after = (arg_max_ipcl_l < sub_len_l - 2) ? fabs(ipcl[arg_max_ipcl_l + 1]) : 0.0;

                        if (debug_count < MAX_DEBUG_LOGS) {
                            int base = debug_count * 9;
                            debug_buffer[base + 0] = -1.0; 
                            debug_buffer[base + 1] = (double)k_l;
                            debug_buffer[base + 2] = (double)start_idx;
                            debug_buffer[base + 3] = (double)e;
                            debug_buffer[base + 4] = (double)(arg_max_ipcl_l + start_idx); 
                            debug_buffer[base + 5] = ipcl_max;
                            debug_buffer[base + 6] = (double)cp_order;
                            debug_buffer[base + 7] = ipcl_before;
                            debug_buffer[base + 8] = ipcl_after;
                            debug_count++;
                        }
                        if(ipcl_max <= thr_fin) { k_r++; k_l++; }
                        
                        delete[] x_temp_l; delete[] ipcl;
                    } else { k_r++; k_l++; }
                }
                delete[] x_temp_r; delete[] ipcr;
            } else { k_r++; k_l++; }
        }
    }
    
    delete[] right_points; delete[] left_points;
    
    if (chp != 0) {
      if (chp > ((e + s)/2)) { 
            pcm_th(x, length_x, thr_const, s, chp, points, 1, k_r, cpt_out, cpt_count, cp_order);
        }
      else{
            int next_kl = (k_l - 1 > 1) ? k_l - 1 : 1;
            pcm_th(x, length_x, thr_const, chp + 1, e, points, next_kl, 1, cpt_out, cpt_count, cp_order);
        }
      
        cpt_out[cpt_count++] = chp; 
    }
}