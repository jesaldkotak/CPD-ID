#ifndef PCM_TH_H
#define PCM_TH_H
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

std::vector<int> pcm_th(float x[], int length_x, float sigma, float thr_const, int s, int e, int points, int k_l, int k_r);

#endif
