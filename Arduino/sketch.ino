#include <Arduino_RouterBridge.h>
#include <vector>
#include <math.h>

// Increase this to match the 2000 points Python is sending
#define MAX_POINTS 2000 
double input_buffer[MAX_POINTS];      
double result_buffer[MAX_POINTS];     
int current_index = 0;

void calculate_cusum(int n) {
    double total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += input_buffer[i]; 
    }

    double running_sum = 0; 
    double fn = (double)n;

    for (int i = 1; i < n; i++) {
        running_sum += input_buffer[i - 1]; 
        double fi = (double)i;
        
        // CUSUM formula with full double precision
        double term1 = sqrt((fn - fi) / (fn * fi)) * running_sum;
        double term2 = sqrt(fi / (fn * (fn - fi))) * (total_sum - running_sum);
        
        result_buffer[i - 1] = term1 - term2;
    }
}

void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size(); 

    // Indexing for 8-byte doubles
    if ((current_index * 8) + bytes_to_copy > (MAX_POINTS * 8)) return;

    memcpy((uint8_t*)input_buffer + (current_index * 8), chunk.data(), bytes_to_copy);
    current_index += (bytes_to_copy / 8); 
}

std::vector<uint8_t> compute_and_get(int total_n) {
    // This check was failing because total_n (2000) was > MAX_POINTS (1000)
    if (total_n > MAX_POINTS || total_n <= 1) return {};

    calculate_cusum(total_n);
    current_index = 0; 
    
    std::vector<uint8_t> output((total_n - 1) * 8);            
    memcpy(output.data(), result_buffer, (total_n - 1) * 8);   
    
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
