#include <Arduino_RouterBridge.h>
#include <vector>
#include "pcm_th.h"

#define MAX_POINTS 2000
float input_buffer[MAX_POINTS];
int current_index = 0;

// --- DEBUG LOGGING ALLOCATION ---
#define MAX_DEBUG_LOGS 1000
float debug_buffer[MAX_DEBUG_LOGS * 7]; // Now 7 floats per entry
int debug_count = 0;
// --------------------------------

void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size(); 
    if ((current_index * 4) + bytes_to_copy > (MAX_POINTS * 4)) return;
    memcpy((uint8_t*)input_buffer + (current_index * 4), chunk.data(), bytes_to_copy);
    current_index += (bytes_to_copy / 4);
}

std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n > MAX_POINTS || total_n <= 0) return {};
    
    int detected_cpts[100];
    int cpt_count = 0;
    int cp_order = 0; // Initialize order tracker
    float thr_const = 1.1; // Updated to match your R code!
    int points = 3;
    
    pcm_th(input_buffer, total_n, thr_const, 1, total_n, points, 1, 1, detected_cpts, cpt_count, cp_order);
    current_index = 0; 
    
    for (int i = 0; i < cpt_count - 1; i++) {
        for (int j = 0; j < cpt_count - i - 1; j++) {
            if (detected_cpts[j] > detected_cpts[j + 1]) {
                int temp = detected_cpts[j]; detected_cpts[j] = detected_cpts[j + 1]; detected_cpts[j + 1] = temp;
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
    int cp_order = 0; // Initialize order tracker
    float thr_const = 1.1; 
    int points = 3;
    
    debug_count = 0; 
    pcm_th(input_buffer, total_n, thr_const, 1, total_n, points, 1, 1, detected_cpts, cpt_count, cp_order);
    current_index = 0; 
    
    // Return 7 floats per entry
    std::vector<uint8_t> output(debug_count * 7 * sizeof(float));
    memcpy(output.data(), debug_buffer, debug_count * 7 * sizeof(float));
    return output;
}

void setup() {
    Bridge.begin();
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("compute_and_get", compute_and_get); 
    Bridge.provide("run_and_log_cusum", run_and_log_cusum); 
}

void loop() { delay(1); }
