#include <Arduino_RouterBridge.h>
#include <vector>
#include <math.h>

// Change double to float to match Python's 4-byte float (struct 'f')
#define MAX_POINTS 2000
float input_buffer[MAX_POINTS];      
float result_buffer[MAX_POINTS];     
int current_index = 0;

// CUSUM calculation logic matching IDetect::cusum_function
void calculate_cusum(int n) {
    double total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += (double)input_buffer[i]; 
    }

    double running_sum = 0; 
    double fn = (double)n;

    // The CUSUM transform for change-point detection (length n-1)
    for (int i = 1; i < n; i++) {
        running_sum += (double)input_buffer[i - 1]; 

        double fi = (double)i;
        
        // Mathematical formula used by IDetect:
        // C_i = sqrt((n-i)/(n*i)) * S_i - sqrt(i/(n*(n-i))) * (S_total - S_i)
        double term1 = sqrt((fn - fi) / (fn * fi)) * running_sum;
        double term2 = sqrt(fi / (fn * (fn - fi))) * (total_sum - running_sum);
        
        result_buffer[i - 1] = (float)(term1 - term2);
    }
}

// Correctly receiving 4-byte floats from Python
void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size(); 

    // Safety check for buffer overflow (4 bytes per float)
    if ((current_index * 4) + bytes_to_copy > (MAX_POINTS * 4)) return;

    // Copying bytes directly into the float buffer
    memcpy((uint8_t*)input_buffer + (current_index * 4), chunk.data(), bytes_to_copy);
    
    current_index += (bytes_to_copy / 4); 
}

std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n > MAX_POINTS || total_n <= 1) return {};

    calculate_cusum(total_n);

    current_index = 0; 
    
    // Result size is (n-1) elements, each 4 bytes
    std::vector<uint8_t> output((total_n - 1) * 4);            
    memcpy(output.data(), result_buffer, (total_n - 1) * 4);   
    
    return output;
}

void setup() {
    Bridge.begin();
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("compute_and_get", compute_and_get);
}

void loop() {
    delay(1); 
}
