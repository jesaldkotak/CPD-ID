
#ifndef PCM_TH_H
#define PCM_TH_H
// We pass cpt_out[] and cpt_count to store the results instead of returning a vector
void pcm_th(double x[], int length_x, double thr_const, int s, int e, int points, int k_l, int k_r, int cpt_out[], int& cpt_count);
#endif
