#include <Arduino_RouterBridge.h>
#include <vector>
#include <math.h>

void cusum_function(float x[], int length, float res[]) {
    // Using a fixed size or local buffer is safer
    float y[length]; 
    y[0] = x[0];
    for (int i = 1; i < length; i++) {
        y[i] = y[i-1] + x[i];
    }
    for (int i = 1; i < length; i++) {
        res[i-1] = sqrtf((length-i)/((float)length*i))*y[i-1] - sqrtf(i/((float)length*(length-i)))*(y[length-1]-y[i-1]);
    }
}

// No Bridge.println here, just return the data
std::vector<float> run_calc() {
    float x_temp[] = { 2.0, 2.0, 2.0, 2.0, 3.0, 3.0 };
    int len = 6;
    float ipc[5];
    
    cusum_function(x_temp, len, ipc);

    std::vector<float> result_vec;
    for(int i = 0; i < 5; i++) {
        result_vec.push_back(ipc[i]);
    }
    
    return result_vec;
}
void setup() {
    Bridge.begin();
    Bridge.provide("run_calc", run_calc);
}
void loop() {
}