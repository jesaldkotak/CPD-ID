#include "pcm_th.h"
#include "s_e_points.h"
#include "cusum_calc.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>


std::vector<int> pcm_th(float x[], int length_x, float sigma, float thr_const, int s, int e, int points, int k_l, int k_r) {
	
	std::vector<int> cpt;									//to store change points
	float thr_fin = thr_const * sigma * sqrt(2*log(length_x));

	if (e - s <= 1) {
		return cpt;
	}
	
	int num_seq = length_x / points;
	std::vector<int> r_e_points(num_seq);					//to store right end points
	std::vector<int> l_e_points(num_seq);					//to store left end points


	for (int i = 0; i < num_seq; i++){
		r_e_points[i] = (i+1) * points;
		l_e_points[i] = length_x - points + 1 - i * points;
	}

	int chp = 0; 
	std::vector<int> right_points, left_points;				//to store right and left points
	int lur = 0, rur = 0;

	s_e_points(r_e_points.data(), l_e_points.data(), s, e, num_seq, right_points.data(), rur, left_points.data(), lur);	//what does .data do?

	if (k_r < k_l) {
		while ((chp == 0) && k_r < k_l && k_r < rur) {
			int end_idx = right_points[k_r - 1];			//last point in right_points
			int sub_len = end_idx - s + 1;					//length of the IUT
			std::vector<float> x_temp_r(sub_len);			//defined the datatype

			for (int i = 0; i < sub_len; i++) {
				x_temp_r[i] = x[s + i - 1];					//x(s) to x(right_points[k_r])
			}
			
			std::vector<float> ipcr(sub_len - 1);			//defined the datatype
			cusum_function(x_temp_r.data(), sub_len, ipcr.data());

			//float ipcr_max[];
			//int pos_r[];
			//ipcr_max[k_r] = ipcr[0];
			float ipcr_max = -1.0;
			int arg_max_ipcr = 0;
			for (int i = 0; i < sub_len - 1; i++) {
				if (std::abs(ipcr[i]) > ipcr_max) {
					ipcr_max = std::abs(ipcr[i]);
					arg_max_ipcr = i;
				}
			}
			//arg_max_ipcr += s - 1;
			//pos_r[k_r] = arg_max_ipcr + s - 1;
			//float CUSUM_r[];
			//CUSUM_r[k_r] = abs(ipcr[pos_r[k_r] + s - 1]);
			//if(CUSUM_r[k_r] > thr_fin) {
			if(ipcr_max > thr_fin){
				chp = arg_max_ipcr + s;
			}
			else {
				k_r++;
			}
		}
	}

	if (k_l < k_r) {
		while ((chp == 0) && k_l < k_r && k_l < lur) {
			int start_idx = left_points[k_l - 1];			//last point in left_points
			int sub_len = e - start_idx + 1;				//length of the IUT
			std::vector<float> x_temp_l(sub_len);			//defined the datatype

			for (int i = 0; i < sub_len; i++) {
				x_temp_l[i] = x[start_idx + i - 1];			//end_idx + e - end_idx + 1 = e, so x(end_idx) to x(e)
			}
			std::vector<float> ipcl(sub_len - 1);
			cusum_function(x_temp_l.data(), e - left_points[k_l] + 1, ipcl.data());

			float ipcl_max = -1.0;
			int arg_max_ipcl = 0;
			//ipcl_max[k_l] = ipcl[0];
			//int arg_max_ipcl = 0;
			for (int i = 0; i < sub_len - 1; i++) {
				if (std::abs(ipcl[i]) > ipcl_max) {
					ipcl_max = std::abs(ipcl[i]);
					arg_max_ipcl = i;
				}
			}

			//arg_max_ipcl += s - 1;
			//pos_l[k_l] = arg_max_ipcl + left_points[k_l] - 1;
			//float CUSUM_l[];
			//CUSUM_l[k_l] = abs(ipcl[pos_l[k_l] - left_points[k_l] + 1]);
			//if (CUSUM_l[k_l] > thr_fin) {
			//	chp = pos_l[k_l];
			//}
			//else {
			//	k_l++;
			//}

			if (ipcl_max > thr_fin) {
				chp = arg_max_ipcl + start_idx;
			}
			else {
				k_l++;
			}
		}
	}

	if (chp == 0) {
		while ((chp == 0) && k_l <= lur && k_r <= rur) {
			int end_idx = right_points[k_r - 1];			//last point in right_points
			int sub_len = end_idx - s + 1;					//length of the IUT
			std::vector<float> x_temp_r(sub_len);			//defined the datatype

			for (int i = 0; i < sub_len; i++) {
				x_temp_r[i] = x[s + i - 1];					//x(s) to x(right_points[k_r])
			}

			std::vector<float> ipcr(sub_len - 1);			//defined the datatype
			cusum_function(x_temp_r.data(), sub_len, ipcr.data());

			float ipcr_max = -1.0;
			int arg_max_ipcr = 0;
			for (int i = 0; i < sub_len - 1; i++) {
				if (std::abs(ipcr[i]) > ipcr_max) {
					ipcr_max = std::abs(ipcr[i]);
					arg_max_ipcr = i;
				}
			}
			if (ipcr_max > thr_fin) {
				chp = arg_max_ipcr + s;
			}
			else {
				int start_idx = left_points[k_l - 1];			//last point in left_points
				int sub_len = e - start_idx + 1;				//length of the IUT
				std::vector<float> x_temp_l(sub_len);			//defined the datatype

				for (int i = 0; i < sub_len; i++) {
					x_temp_l[i] = x[start_idx + i - 1];			//end_idx + e - end_idx + 1 = e, so x(end_idx) to x(e)
				}
				std::vector<float> ipcl(sub_len - 1);
				cusum_function(x_temp_l.data(), e - left_points[k_l] + 1, ipcl.data());

				float ipcl_max = -1.0;
				int arg_max_ipcl = 0;
				for (int i = 0; i < sub_len - 1; i++) {
					if (std::abs(ipcl[i]) > ipcl_max) {
						ipcl_max = std::abs(ipcl[i]);
						arg_max_ipcl = i;
					}
				}

				if (ipcl_max > thr_fin) {
					chp = arg_max_ipcl + start_idx;
				}
				else {
					k_r++;
					k_l++;
				}
			}
		}
	}
	if (chp != 0) {
		cpt.push_back(chp);					//push_back is used to add an element to the end of the vector
		std::vector<int> next_cpts;			//to store change points in the right segment
		if(chp > (e+s)/2){
			next_cpts = pcm_th(x, length_x, sigma, thr_const, s, chp, points, 1, k_r);
		}
		else {
			if(k_l - 1 > 1)
				next_cpts = pcm_th(x, length_x, sigma, thr_const, chp + 1, e, points, k_l - 1, 1);
			else
				next_cpts = pcm_th(x, length_x, sigma, thr_const, chp + 1, e, points, 1, 1);
		}
		cpt.insert(cpt.end(), next_cpts.begin(), next_cpts.end()); //concatenate chp and cpt_r //begin and end tells us where to start and end in the vector
			
	}
	
	std::sort(cpt.begin(), cpt.end());	//sort the change points in ascending order
	return cpt;
}