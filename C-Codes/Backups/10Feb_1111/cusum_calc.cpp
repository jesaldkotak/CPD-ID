#include "cusum_calc.h"
#include<math.h>


void cusum_function(float x[], int length, float res[]) {
	const int MAX_LEN = 256;
	float y[MAX_LEN] = {0.0f};

	y[0] = x[0];
	for (int i = 1; i < length; i++) {
		y[i] = y[i-1] + x[i];
	}

	for (int i = 1; i < length; i++) {
		res[i-1] = sqrtf( (length-i)/((float)length*i) )*y[i-1] - sqrtf( i/((float)length*(length-i)) )*(y[length-1]-y[i-1]);
	}
}