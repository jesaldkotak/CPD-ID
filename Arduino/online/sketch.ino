#include <Arduino_RouterBridge.h>
#include <vector>
#include <stdint.h>
#include "pcm_th.h"

#define MAX_POINTS 2000
float input_buffer[MAX_POINTS];
int current_index = 0;

// --------------------------------------------------------------------------
// Online detection state.
//
// In a real deployment this data would arrive from GPIO sampling, one point
// (or one small chunk) at a time, with pcm_th run only once the buffer has
// been updated with a full new chunk. We're standing in for that here by
// having Python stream chunks of `points` size, and running pcm_th once per
// chunk, right after that chunk lands in input_buffer.
//
// s/e track the window pcm_th is currently testing, and persist across
// stream_data calls (this is genuinely sequential state, not something we
// can recompute from scratch each call). detected_cpts/cpt_count accumulate
// every changepoint found so far for the current column.
//
// NOTE: no reset between columns yet -- this only handles one column at a
// time for now, as agreed.
// --------------------------------------------------------------------------
const int points = 3;
float thr_const = 2.1;

int s = 1;
int e = points;

int detected_cpts[100];
int cpt_count = 0;

// Reset all persistent detection state between columns. Must be called from
// Python before streaming a new column, since these globals otherwise carry
// over from the previous column's run.
std::vector<uint8_t> reset_state(std::vector<uint8_t>) {
    current_index = 0;
    s = 1;
    e = points;
    cpt_count = 0;
    memset(detected_cpts, 0, sizeof(detected_cpts));
    return {};
}

std::vector<uint8_t> stream_data(std::vector<uint8_t> chunk) {
    if (chunk.empty()) return {};
    int bytes_to_copy = chunk.size();
    if ((current_index * 4) + bytes_to_copy > (MAX_POINTS * 4)) return {};
    memcpy((uint8_t*)input_buffer + (current_index * 4), chunk.data(), bytes_to_copy);
    current_index += (bytes_to_copy / 4);

    // Run pcm_th once now that the buffer has been updated with this chunk,
    // timing just the pcm_th call itself (pure compute, excludes the memcpy
    // above and the bridge transport on either side).
    int before = cpt_count;
    unsigned long t_start = micros();
    pcm_th(input_buffer, thr_const, s, e, points, detected_cpts, cpt_count);
    unsigned long t_end = micros();
    int32_t exec_time_us = (int32_t)(t_end - t_start);

    int32_t found_flag = 0;
    int32_t cp_index = 0;
    int32_t s_detected = s;   // capture before any update
    int32_t e_detected = e;   // capture before any update

    if (cpt_count > before) {
        // CP found: restart window from immediately after the current e,
        // so we only ever search completely new (unseen) data next.
        int chp = detected_cpts[cpt_count - 1];
        s = e + 1;
        e = s + points - 1;
        found_flag = 1;
        cp_index = chp;
    } else {
        // No CP: grow the window by one chunk.
        e = e + points;
        // Rolling window clamp: keep window width <= 1000 by advancing s.
        // Once e - s + 1 > 1000, s moves up so the width stays at 1000.
        if (e - s + 1 > 1000) {
            s = e - 1000 + 1;
        }
    }

    // Always return a fixed 20-byte response:
    // [exec_time_us, found_flag, cp_index, s_at_detection, e_at_detection]
    // s_at_detection and e_at_detection are the window [s,e] that was active
    // when this chunk was tested -- meaningful on every chunk, but Python
    // only records them when found_flag is set.
    std::vector<uint8_t> output(5 * sizeof(int32_t));
    memcpy(output.data() + 0 * sizeof(int32_t), &exec_time_us,  sizeof(int32_t));
    memcpy(output.data() + 1 * sizeof(int32_t), &found_flag,    sizeof(int32_t));
    memcpy(output.data() + 2 * sizeof(int32_t), &cp_index,      sizeof(int32_t));
    memcpy(output.data() + 3 * sizeof(int32_t), &s_detected,    sizeof(int32_t));
    memcpy(output.data() + 4 * sizeof(int32_t), &e_detected,    sizeof(int32_t));
    return output;
}

void setup() {
    Bridge.begin();
    Bridge.provide("stream_data", stream_data);
    Bridge.provide("reset_state", reset_state);
}

void loop() { delay(1); }
