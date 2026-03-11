#include <Arduino_RouterBridge.h>
#include "pcm_th.h"

std::vector<uint8_t> run_pcm_th_test() {
    float test_data[] = { 2.0, 2.0, 2.0, 2.0, 2.0, 5.0, 5.0, 5.0, 5.0, 8.0, 8.0, 8.0, 8.0};
    int length = sizeof(test_data) / sizeof(test_data[0]);
    float thr_const = 1.3;
    int points = 2;

    int detected_cpts[20];
    int cpt_count = 0;

    pcm_th(test_data, length, thr_const, 1, length, points, 1, 1, detected_cpts, cpt_count);

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

void setup() {
    Bridge.begin();
    Bridge.provide("run_pcm_th_test", run_pcm_th_test);
}

void loop() {
    delay(1); 
}