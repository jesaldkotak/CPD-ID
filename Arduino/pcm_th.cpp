#include "pcm_th.h"
#include "s_e_points.h"
#include <math.h>

extern float debug_buffer[];
extern int debug_count;
#define MAX_DEBUG_LOGS 1000

static void calculate_cusum_local(float x[], int length, float res[]) {
    if (length < 2) return;
    float* y = new float[length];
    y[0] = x[0];
    for (int i = 1; i < length; i++) y[i] = y[i-1] + x[i];
    float fn = (float)length;
    for (int i = 1; i < length; i++) {
        float fi = (float)i;
        res[i-1] = sqrtf((fn - fi) / (fn * fi)) * y[i-1] - sqrtf(fi / (fn * (fn - fi))) * (y[length-1] - y[i-1]);
    }
    delete[] y;
}

static void sort_float(float arr[], int n) {
    for(int i=0; i<n-1; i++) {
        for(int j=0; j<n-i-1; j++) {
            if(arr[j] > arr[j+1]) { float t=arr[j]; arr[j]=arr[j+1]; arr[j+1]=t; }
        }
    }
}

static float median(float arr[], int n) {
    float* temp = new float[n];
    for(int i=0; i<n; i++) temp[i] = arr[i];
    sort_float(temp, n);
    float res;
    if (n % 2 == 0) res = (temp[n / 2 - 1] + temp[n / 2]) / 2.0f;
    else res = temp[n / 2];
    delete[] temp;
    return res;
}

static float estimate_sigma(float x[], int n) {
    if (n < 2) return 0.0f;
    float* diffs = new float[n - 1];
    for (int i = 1; i < n; ++i) diffs[i - 1] = (x[i] - x[i - 1]) / sqrtf(2.0f);
    float med = median(diffs, n - 1);
    
    float* abs_devs = new float[n - 1];
    for (int i = 0; i < n - 1; ++i) abs_devs[i] = fabsf(diffs[i] - med);
    float sigma = 1.4826f * median(abs_devs, n - 1);
    
    delete[] diffs; delete[] abs_devs;
    return sigma;
}

// Added cp_order reference here!
void pcm_th(float x[], int length_x, float thr_const, int s, int e, int points, int k_l, int k_r, int cpt_out[], int& cpt_count, int& cp_order) {
    
    float sigma = estimate_sigma(x, length_x);
    float thr_fin = thr_const * sigma * sqrtf(2.0f * logf((float)length_x));

    if (thr_fin <= 1e-5f) thr_fin = 1e-5f;

    // Expand special entry to 7 slots
    if (debug_count == 0) {
        int base = debug_count * 7;
        debug_buffer[base + 0] = 0.0f; 
        debug_buffer[base + 1] = 0.0f;
        debug_buffer[base + 2] = 0.0f;
        debug_buffer[base + 3] = 0.0f;
        debug_buffer[base + 4] = 0.0f;
        debug_buffer[base + 5] = thr_fin; // Keep threshold here
        debug_buffer[base + 6] = 0.0f;
        debug_count++;
    }

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

    delete[] r_e_points; delete[] l_e_points;

    if (k_r < k_l) {
        while ((chp == 0) && k_r < k_l && k_r < rur) {
            int end_idx = right_points[k_r - 1];
            int sub_len = end_idx - s + 1;
            if (sub_len >= 2) {
                float* x_temp_r = new float[sub_len];
                float* ipcr = new float[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_r[i] = x[s + i - 1];
                calculate_cusum_local(x_temp_r, sub_len, ipcr);

                float ipcr_max = -1.0f; int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabsf(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabsf(ipcr[i]); arg_max_ipcr = i;
                    }
                }
            
                if(ipcr_max > thr_fin) {
                    chp = arg_max_ipcr + s;
                    cp_order++; 
                }
                  if (debug_count < MAX_DEBUG_LOGS) {
                    int base = debug_count * 7;
                    debug_buffer[base + 0] = 1.0f; 
                    debug_buffer[base + 1] = (float)k_r;
                    debug_buffer[base + 2] = (float)s; 
                    debug_buffer[base + 3] = (float)end_idx; 
                    debug_buffer[base + 4] = (float)(arg_max_ipcr + s);
                    debug_buffer[base + 5] = ipcr_max;
                    debug_buffer[base + 6] = (float)cp_order;
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
                float* x_temp_l = new float[sub_len];
                float* ipcl = new float[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_l[i] = x[start_idx + i - 1];
                calculate_cusum_local(x_temp_l, sub_len, ipcl);

                float ipcl_max = -1.0f; int arg_max_ipcl = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabsf(ipcl[i]) > ipcl_max) {
                        ipcl_max = fabsf(ipcl[i]); arg_max_ipcl = i;
                    }
                }

                
              
                if (ipcl_max > thr_fin) {
                    chp = arg_max_ipcl + start_idx;
                    cp_order++;
                }
                  if (debug_count < MAX_DEBUG_LOGS) {
                    int base = debug_count * 7;
                    debug_buffer[base + 0] = -1.0f; // Left
                    debug_buffer[base + 1] = (float)k_l;
                    debug_buffer[base + 2] = (float)start_idx; // Start_Index
                    debug_buffer[base + 3] = (float)e;         // End_Index
                    debug_buffer[base + 4] = (float)(arg_max_ipcl + start_idx);
                    debug_buffer[base + 5] = ipcl_max;
                    debug_buffer[base + 6] = (float)cp_order;
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
                float* x_temp_r = new float[sub_len];
                float* ipcr = new float[sub_len];
                for (int i = 0; i < sub_len; i++) x_temp_r[i] = x[s + i - 1];
                calculate_cusum_local(x_temp_r, sub_len, ipcr);

                float ipcr_max = -1.0f; int arg_max_ipcr = 0;
                for (int i = 0; i < sub_len - 1; i++) {
                    if (fabsf(ipcr[i]) > ipcr_max) {
                        ipcr_max = fabsf(ipcr[i]); arg_max_ipcr = i;
                    }
                }

                
                
                if (ipcr_max > thr_fin) {
                    chp = arg_max_ipcr + s;
                    cp_order++;
                }
                  if (debug_count < MAX_DEBUG_LOGS) {
                      int base = debug_count * 7;
                      debug_buffer[base + 0] = 1.0f; // Right
                      debug_buffer[base + 1] = (float)k_r;
                      debug_buffer[base + 2] = (float)s;
                      debug_buffer[base + 3] = (float)end_idx;
                      debug_buffer[base + 4] = (float)(arg_max_ipcr + s);
                      debug_buffer[base + 5] = ipcr_max;
                      debug_buffer[base + 6] = (float)cp_order;
                      debug_count++;
                  }
                if (ipcr_max <= thr_fin) {
                    int start_idx = left_points[k_l - 1];
                    int sub_len_l = e - start_idx + 1;
                    
                    if (sub_len_l >= 2) {
                        float* x_temp_l = new float[sub_len_l];
                        float* ipcl = new float[sub_len_l];
                        for (int i = 0; i < sub_len_l; i++) x_temp_l[i] = x[start_idx + i - 1];
                        calculate_cusum_local(x_temp_l, sub_len_l, ipcl);

                        float ipcl_max = -1.0f; int arg_max_ipcl_l = 0;
                        for (int i = 0; i < sub_len_l - 1; i++) {
                            if (fabsf(ipcl[i]) > ipcl_max) {
                                ipcl_max = fabsf(ipcl[i]); arg_max_ipcl_l = i;
                            }
                        }

                        

                        if (ipcl_max > thr_fin) {
                            chp = arg_max_ipcl_l + start_idx;
                            cp_order++;
                        }
                          if (debug_count < MAX_DEBUG_LOGS) {
                            int base = debug_count * 7;
                            debug_buffer[base + 0] = -1.0f; // Left
                            debug_buffer[base + 1] = (float)k_l;
                            debug_buffer[base + 2] = (float)start_idx;
                            debug_buffer[base + 3] = (float)e;
                            debug_buffer[base + 4] = (float)(arg_max_ipcl_l + start_idx); 
                            debug_buffer[base + 5] = ipcl_max;
                            debug_buffer[base + 6] = (float)cp_order;
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
    
        if ((chp - 1) - s >= points) {
            pcm_th(x, length_x, thr_const, s, chp, points, 1, k_r, cpt_out, cpt_count, cp_order);
        }
    
        if (e - (chp + 1) >= points) {
            int next_kl = (k_l - 1 > 1) ? k_l - 1 : 1;
            pcm_th(x, length_x, thr_const, chp + 1, e, points, next_kl, 1, cpt_out, cpt_count, cp_order);
        }
      
        cpt_out[++cpt_count] = chp; 
    }
}
