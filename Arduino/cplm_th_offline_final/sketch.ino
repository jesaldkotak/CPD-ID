#include <Arduino_RouterBridge.h>
#include <vector>
#include "pcm_th.h"

#define MAX_POINTS 2000
double input_buffer[MAX_POINTS];  // Changed to double
int current_index = 0;

// --- DEBUG LOGGING ALLOCATION ---
#define MAX_DEBUG_LOGS 1000
double debug_buffer[MAX_DEBUG_LOGS * 9]; // Changed to double
int debug_count = 0;
// --------------------------------

void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size();
    
    // Now multiplying by 8 because a double is 8 bytes
    if ((current_index * 8) + bytes_to_copy > (MAX_POINTS * 8)) return;
    memcpy((uint8_t*)input_buffer + (current_index * 8), chunk.data(), bytes_to_copy);
    
    current_index += (bytes_to_copy / 8); // Divide by 8
}

std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n > MAX_POINTS || total_n <= 0) return {};
    
    int detected_cpts[100];
    int cpt_count = 0;
    int cp_order = 0;
    
    double thr_const = 1.4; // Changed to double
    int points = 10;
    
    pcm_th(input_buffer, total_n, thr_const, 1, total_n, points, 1, 1, detected_cpts, cpt_count, cp_order);
    current_index = 0;
    
    for (int i = 0; i < cpt_count - 1; i++) {
        for (int j = 0; j < cpt_count - i - 1; j++) {
            if (detected_cpts[j] > detected_cpts[j + 1]) {
                int temp = detected_cpts[j];
                detected_cpts[j] = detected_cpts[j + 1]; 
                detected_cpts[j + 1] = temp;
            }
        }
    }

    std::vector<uint8_t> output(cpt_count * sizeof(int));
    memcpy(output.data(), detected_cpts, cpt_count * sizeof(int));
    return output;
}

std::vector<uint8_t> run_and_log_cusum(int total_n) {
    if (total_n > MAX_POINTS || total_n <= 0) return {};
    
    int detected_cpts[100];
    int cpt_count = 0;
    int cp_order = 0;
    
    double thr_const = 1.4; // Changed to double
    int points = 10;
    
    debug_count = 0;
    
    pcm_th(input_buffer, total_n, thr_const, 1, total_n, points, 1, 1, detected_cpts, cpt_count, cp_order);
    current_index = 0;
    
    // Return using sizeof(double)
    std::vector<uint8_t> output(debug_count * 9 * sizeof(double));
    memcpy(output.data(), debug_buffer, debug_count * 9 * sizeof(double));
    return output;
}

void setup() {
    Bridge.begin();
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("compute_and_get", compute_and_get);
    Bridge.provide("run_and_log_cusum", run_and_log_cusum); 
}

void loop() { delay(1); }
