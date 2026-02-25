#include<iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "cusum_calc.h"
#include "s_e_points.h"
#include "pcm_th.h"

float median(float arr[], int n) {
	std::vector<float> vec(arr, arr + n);
	std::sort(vec.begin(), vec.end());
	if (n % 2 == 0) {
		return (vec[n / 2 - 1] + vec[n / 2]) / 2.0;
	} else {
		return vec[n / 2];
	}
}

float estimate_sigma(float x[], int n) {
	if (n < 2) return 0.0f;

	std::vector<float> diffs(n - 1);
	for (int i = 1; i < n; ++i) {
		diffs[i - 1] = (x[i] - x[i - 1]) / std::sqrt(2.0f);
	}
	
	float med = median(diffs.data(), n - 1);

	std::vector<float> abs_devs(n - 1);
	for (int i = 0; i < n - 1; ++i) {
		abs_devs[i] = std::fabs(diffs[i] - med);
	}

	return 1.4826f * median(abs_devs.data(), n - 1);
}

int main()
{
	float x_temp[] = { 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3 };
	int length = sizeof(x_temp) / sizeof(x_temp[0]);

	//std::cout << length << std::endl;

	std::vector<int> cpt_pcm_th = pcm_th(x_temp, length, estimate_sigma(x_temp,length), 1.0, 1, length, 2, 1, 1);
	//pcm_th(float x[], int length_x, float sigma, float thr_const, int s, int e, int points, int k_l, int k_r) 
	
	if (cpt_pcm_th.empty()) {
		std::cout << "No change points detected." << std::endl;
	}
	else{
		for (int i = 0; i < cpt_pcm_th.size(); i++) {
			std::cout << cpt_pcm_th[i] << "  ";
		}
		std::cout << std::endl;
	}

	return 0;
}