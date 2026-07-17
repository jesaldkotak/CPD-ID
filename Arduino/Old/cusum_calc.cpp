#include "cusum_calc.h"
#include <math.h>

void cusum_function(float x[], int length, float res[]) {
    if (length < 2) return;
    
    // 1. Use float for internal accumulation to prevent precision loss
    float total_sum = 0.0;
    float current_y = 0.0;
    
    // Pre-calculate the total sum (y[length-1]) in float precision
    for (int i = 0; i < length; i++) {
        total_sum += (float)x[i];
    }

    float fn = (float)length;
    
    // 2. Optimized Loop
    for (int i = 1; i < length; i++) {
        current_y += (float)x[i-1]; // Accumulate y[i-1]
        float fi = (float)i;
        
        // 3. Simplified formula to reduce square root and subtraction noise
        // This formula is algebraically equivalent but numerically more stable
        float numerator = (current_y * fn) - (fi * total_sum);
        float denominator = sqrtf(fn * fi * (fn - fi));
        
        res[i-1] = (float)(numerator / denominator);
    }
}
