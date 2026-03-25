#include "cusum_calc.h"
#include <math.h>

void cusum_function(float x[], int length, float res[]) {
    if (length < 2) return;
    
    float y[100] = {0.0f}; // Safe for small datasets <= 100
    
    y[0] = x[0];
    for (int i = 1; i < length; i++) {
        y[i] = y[i-1] + x[i];
    }

    float fn = (float)length;
    for (int i = 1; i < length; i++) {
        float fi = (float)i;
        res[i-1] = sqrtf((fn - fi) / (fn * fi)) * y[i-1] - sqrtf(fi / (fn * (fn - fi))) * (y[length-1] - y[i-1]);
    }
}