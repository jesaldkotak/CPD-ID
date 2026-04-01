#include <Arduino_RouterBridge.h>
#include "pcm_th.h"

#define MAX_POINTS 2000
double input_buffer[MAX_POINTS]; // Changed to double
int current_index = 0;

void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size(); 

    // Safety check for 8-byte double precision
    if ((current_index * 8) + bytes_to_copy > (MAX_POINTS * 8)) return;

    // Copy chunks into our global array at 8-byte intervals
    memcpy((uint8_t*)input_buffer + (current_index * 8), chunk.data(), bytes_to_copy);
    current_index += (bytes_to_copy / 8);
}

std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n > MAX_POINTS || total_n <= 0) return {};

    int detected_cpts[100]; 
    int cpt_count = 0;
    
    // Tuning parameters for noisy data
    double thr_const = 1.3; // Updated to double
    int points = 10; 

    // Run the algorithm
    pcm_th(input_buffer, total_n, thr_const, 1, total_n, points, 1, 1, detected_cpts, cpt_count);

    current_index = 0; 

    // Bubble sort the results chronologically
    for (int i = 0; i < cpt_count - 1; i++) {
        for (int j = 0; j < cpt_count - i - 1; j++) {
            if (detected_cpts[j] > detected_cpts[j + 1]) {
                int temp = detected_cpts[j];
                detected_cpts[j] = detected_cpts[j + 1];
                detected_cpts[j + 1] = temp;
            }
        }
    }

    // Convert the integer results to a byte vector. 
    // We leave this as sizeof(int) because the change points are indices, not data points.
    std::vector<uint8_t> output(cpt_count * sizeof(int));
    memcpy(output.data(), detected_cpts, cpt_count * sizeof(int));
    
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
