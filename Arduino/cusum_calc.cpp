#include "cusum_calc.h"
#include <math.h>

void cusum_function(double x[], int length, double res[]) {
    if (length < 2) return;
    
    // 1. Use double for internal accumulation to prevent precision loss
    double total_sum = 0.0;
    double current_y = 0.0;
    
    // Pre-calculate the total sum (y[length-1]) in double precision
    for (int i = 0; i < length; i++) {
        total_sum += (double)x[i];
    }

    double fn = (double)length;
    
    // 2. Optimized Loop
    for (int i = 1; i < length; i++) {
        current_y += (double)x[i-1]; // Accumulate y[i-1]
        double fi = (double)i;
        
        // 3. Simplified formula to reduce square root and subtraction noise
        // This formula is algebraically equivalent but numerically more stable
        double numerator = (current_y * fn) - (fi * total_sum);
        double denominator = sqrt(fn * fi * (fn - fi));
        
        res[i-1] = (double)(numerator / denominator);
    }
}
