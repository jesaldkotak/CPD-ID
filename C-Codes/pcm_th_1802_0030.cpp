#include "pcm_th.h"
#include "s_e_points.h"
#include "cusum_calc.h"
#include <cmath>

void pcm_th(int x[], int length_x, float sigma, float thr_const = 1, int s = 1, int e, int points = 3, int k_l = 1, int k_r = 1, int cpt[]) {
	
	//int e = length_x; 
	float thr_fin = thr_const * sigma * sqrt(2*log(length_x));
	int r_e_points[], l_e_points[];
	
	for (int i = 1; i <= (length_x / points); i++){
		r_e_points[i] = i * points;
		l_e_points[i] = length_x - points + 1 - (i - 1) * points;
	}

	int chp = 0; 

	if (e - s <= 1) chp = 0;
	else {
		int right_points[], int left_points[];
		int lur, rur;
		int movingpoints[] = s_e_points(r_e_points, l_e_points, s, e, length_x, right_points, rur, left_points, lur);

		if (k_r < k_l) {
			while ((chp == 0) && (k_r < k_l && k_r < rur)) {
				int x_temp_r[];
				for (int i = s; i <= right_points[k_r]; i++) {
					x_temp_r[i] = x[i];
				}
				float ipcr[];
				cusum_function(x_temp_r, right_points[k_r] - s + 1, ipcr);

				float ipcr_max[];
				int pos_r[];
				ipcr_max[k_r] = ipcr[0];
				int arg_max_ipcr = 0;
				for (int i = 1; i < (right_points[k_r] - s); i++) {
					if (ipcr[i] > ipcr_max[k_r]) {
						ipcr_max[k_r] = ipcr[i];
						arg_max_ipcr = i;
					}
				}
				arg_max_ipcr += s - 1;
				pos_r[k_r] = arg_max_ipcr + s - 1;
				float CUSUM_r[];
				CUSUM_r[k_r] = abs(ipcr[pos_r[k_r] + s - 1]);
				if(CUSUM_r[k_r] > thr_fin) {
					chp = pos_r[k_r];
				}
				else {
					k_r++;
				}
			}
		}
		if (k_l < k_r) {
			while ((chp == 0) && (k_l < k_r && k_l < lur)) {
				int x_temp_l[];
				for (int i = left_points[k_l]; i <= e; i++) {
					x_temp_l[i] = x[i];
				}
				float ipcl[];
				cusum_function(x_temp_l, e - left_points[k_l] + 1, ipcl);

				float ipcl_max[];
				int pos_l[];
				ipcl_max[k_l] = ipcl[0];
				int arg_max_ipcl = 0;
				for (int i = 1; i < (e - left_points[k_l]); i++) {
					if (ipcl[i] > ipcl_max[k_l]) {
						ipcl_max[k_l] = ipcl[i];
						arg_max_ipcl = i;
					}
				}
				arg_max_ipcl += s - 1;
				pos_l[k_l] = arg_max_ipcl + left_points[k_l] - 1;
				float CUSUM_l[];
				CUSUM_l[k_l] = abs(ipcl[pos_l[k_l] - left_points[k_l] + 1]);
				if (CUSUM_l[k_l] > thr_fin) {
					chp = pos_l[k_l];
				}
				else {
					k_l++;
				}
			}
		}
		if (chp == 0) {
			while ((chp == 0) && (k_l <= lur && k_r <= rur)) {
				int x_temp_r[];
				for (int i = s; i <= right_points[k_r]; i++) {
					x_temp_r[i] = x[i];
				}
				float ipcr[];
				cusum_function(x_temp_r, right_points[k_r] - s + 1, ipcr);

				float ipcr_max[];
				int pos_r[];
				ipcr_max[k_r] = ipcr[0];
				int arg_max_ipcr = 0;
				for (int i = 1; i < (right_points[k_r] - s); i++) {
					if (ipcr[i] > ipcr_max[k_r]) {
						ipcr_max[k_r] = ipcr[i];
						arg_max_ipcr = i;
					}
				}
				arg_max_ipcr += s - 1;
				pos_r[k_r] = arg_max_ipcr + s - 1;
				float CUSUM_r[];
				CUSUM_r[k_r] = abs(ipcr[pos_r[k_r] + s - 1]);
				if (CUSUM_r[k_r] > thr_fin) {
					chp = pos_r[k_r];
				}
				else {
					int x_temp_l[];
					for (int i = left_points[k_l]; i <= e; i++) {
						x_temp_l[i] = x[i];
					}
					float ipcl[];
					cusum_function(x_temp_l, e - left_points[k_l] + 1, ipcl);

					float ipcl_max[];
					int pos_l[];
					ipcl_max[k_l] = ipcl[0];
					int arg_max_ipcl = 0;
					for (int i = 1; i < (e - left_points[k_l]); i++) {
						if (ipcl[i] > ipcl_max[k_l]) {
							ipcl_max[k_l] = ipcl[i];
							arg_max_ipcl = i;
						}
					}
					arg_max_ipcl += s - 1;
					pos_l[k_l] = arg_max_ipcl + left_points[k_l] - 1;
					float CUSUM_l[];
					CUSUM_l[k_l] = abs(ipcl[pos_l[k_l] - left_points[k_l] + 1]);
					if (CUSUM_l[k_l] > thr_fin) {
						chp = pos_l[k_l];
					}
					else {
						k_r++;
						k_l++;
					}
				}
			}
		}
		if (chp != 0) {
			int cpt_r[];
			if(chp > (e+s)/2){
				pcm_th(x, length_x, sigma, thr_const, s, chp, points, k_l = 1, k_r, cpt_r);
			}
			else {
				if(k_l - 1 > 1)
					pcm_th(x, length_x, sigma, thr_const, chp + 1, e, points, k_l = k_l - 1, k_r = 1, cpt_r);
				else
					pcm_th(x, length_x, sigma, thr_const, chp + 1, e, points, k_l = 1, k_r = 1, cpt_r);
			}
			cpt = c(chp, r); //concatenate chp and r
		}
		else {
			cpt = chp;
		}
	}
	cpt = cpt[cpt != 0];
	return(sort(cpt));
}