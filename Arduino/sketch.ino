#include <Arduino_RouterBridge.h>
#include <vector>
#include <math.h>

// Global buffers use the 786KB SRAM area to prevent stack overflows [cite: 2, 3]
#define MAX_POINTS 2000
float input_buffer[MAX_POINTS];
float result_buffer[MAX_POINTS];
int current_index = 0;

/**
 * Logic aligned with the R function:
 * res = sqrt(((n-i)/n)/i) * y[i] - sqrt(i/n/(n-i)) * (y[n] - y[i])
 */
void calculate_cusum(int n) {
    float total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += input_buffer[i]; 
    }

    float running_y = 0; 
    float fn = (float)n;

    // The R function calculates (n-1) results
    for (int i = 1; i < n; i++) {
        // y[i] in R is the sum of the first i elements
        running_y += input_buffer[i - 1]; 

        float fi = (float)i;
        
        // Match R: sqrt(((n-i)/n)/i) * y[i]
        float term1 = sqrtf((fn - fi) / (fn * fi)) * running_y; 
        
        // Match R: sqrt(i/n/(n-i)) * (y[n] - y[i])
        float term2 = sqrtf(fi / (fn * (fn - fi))) * (total_sum - running_y); 
        
        result_buffer[i - 1] = term1 - term2; 
    }
}
          
// Chunks are received as bytes (uint8_t) to avoid C++ library linker errors 
void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size();
    
    // Safety check against buffer overflow
    if ((current_index * 4) + bytes_to_copy > (MAX_POINTS * 4)) return;
    
    memcpy((uint8_t*)input_buffer + (current_index * 4), chunk.data(), bytes_to_copy);
    current_index += (bytes_to_copy / 4);
}

// Triggers calculation and returns results to Python via the Bridge [cite: 21]
std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n < 2 || total_n > MAX_POINTS) return {};

    calculate_cusum(total_n);

    current_index = 0; // Reset index for the next run
    
    // Pack float results back into a byte vector for transfer [cite: 6]
    int result_count = total_n - 1;
    std::vector<uint8_t> output(result_count * 4);
    memcpy(output.data(), result_buffer, result_count * 4);
    
    return output;
}

void setup() {
    // Standard Bridge initialization [cite: 21]
    Bridge.begin();
    
    // Expose providers for the Python backend to call [cite: 21]
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("compute_and_get", compute_and_get);
}

void loop() {
    // Fast loop to keep the Bridge responsive [cite: 22]
    delay(1); 
}