#include <Arduino_RouterBridge.h>
#include <vector>
#include <math.h>

//To prevent stack overflows- Global buffers use the 786KB SRAM area
#define MAX_POINTS 2000
float input_buffer[MAX_POINTS];      //We have assummed 2000 point input
float result_buffer[MAX_POINTS];     //And result, even though this can be 2000-1
int current_index = 0;

//cusum calculation function: result is stored in global buffer
void calculate_cusum(int n) {
    float total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += input_buffer[i]; //like the cumsum function in R
    }

    float running_y = 0; 
    float fn = (float)n;  //converting int n to float for division/multiplication

    for (int i = 1; i < n; i++) {
        running_y += input_buffer[i - 1]; //Like y[i] in R

        float fi = (float)i;  //converting int i to float for division/multiplication
                
        result_buffer[i - 1] = sqrtf((fn - fi) / (fn * fi)) * running_y - sqrtf(fi / (fn * (fn - fi))) * (total_sum - running_y);//Cusum cal
    }
}

//copying input chunk data into input_buffer, upto MAX_POINTS
//Receiving data in chunks of uint8_t to avoid linker errors
//Note: float = 4 bytes
void stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return;
    int bytes_to_copy = chunk.size();  //this size will be in bytes, so 50*4 = 200 bytes per chunk

    if ((current_index * 4) + bytes_to_copy > (MAX_POINTS * 4)) return;  //this will prevent the code from writing beyond the max buffer size we allocated
  //last current_index = MAX_POINTS = 2000

    
    //address to copy: (uint8_t*)input_buffer + (current_index * 4)
    memcpy((uint8_t*)input_buffer + (current_index * 4), chunk.data(), bytes_to_copy);
    
    current_index += (bytes_to_copy / 4);  //increments current index by 50, so total 2000/50 = 40 iterations of this loop
}

// Triggers calculation and returns results to Python via the Bridge 
std::vector<uint8_t> compute_and_get(int total_n) {
    if (total_n > MAX_POINTS) return {};

    calculate_cusum(total_n);

    current_index = 0; // Reseting the index for the next run
    
    // Packing float results back into a byte vector for transfer 
    std::vector<uint8_t> output((total_n - 1) * 4);            //cusum output size will be total_n-1
    memcpy(output.data(), result_buffer, (total_n - 1) * 4);   //sending all the result data at once
    
    return output;
}

void setup() {
    // Standard Bridge initialization
    Bridge.begin();
    
    // Expose providers for the Python backend to call
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("compute_and_get", compute_and_get);
}

void loop() {
    // Fast loop to keep the Bridge responsive
    delay(1); 
}