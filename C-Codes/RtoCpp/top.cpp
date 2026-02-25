#include<iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "cusum_calc.h"
#include "s_e_points.h"
#include "pcm_th.h"

int main()
{
	float x_temp[] = { 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3 };
	int length = sizeof(x_temp) / sizeof(x_temp[0]);

	//std::cout << length << std::endl;

	std::vector<int> cpt_pcm_th = pcm_th(x_temp, length, 1.0, 1, length, 2, 1, 1);
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