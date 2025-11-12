#ifndef GLOBAL_TIMER_H
#define GLOBAL_TIMER_H

#include <chrono>

static std::chrono::high_resolution_clock::time_point g_start_time;

inline void global_timer_restart() {
    g_start_time = std::chrono::high_resolution_clock::now();
}

inline double global_timer_elapsed() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - g_start_time);
    return duration.count() / 1000000.0;
}

#endif
