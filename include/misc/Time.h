#ifndef __BENCHMARK__
#define __BENCHMARK__

#include <sys/time.h>
#include<SDL2/SDL.h>
#include <iostream>

#include "common/ex.h"
#include "common/helper.h"

class Time {
private:
    timeval m_start, m_end;
    uintmax_t m_delay;

public:
    // Constructor
    Time(uintmax_t delay);

    // Start the benchmark
    void start();

    // Return the difference between start time
    // and stop time in micro seconds
    uintmax_t time();

    // Wait for some time to get consistent delay
    void wait();
};


inline Time::Time(uintmax_t delay) :
    m_delay(delay) {
}

// Start the benchmark time
inline void Time::start() {
    gettimeofday(&m_start, 0);
}

// Return the difference between start time
// and stop time in micro seconds
inline uintmax_t Time::time() {
    gettimeofday(&m_end, 0);
    return (m_end.tv_sec * 1e6 + m_end.tv_usec - m_start.tv_sec * 1e6 - m_start.tv_usec);
}

// Wait for some time to get consistent delay
inline void Time::wait() {
    // get time difference
    uintmax_t t = time();

    // Get the fps
    static float fps = 0;
    fps = (fps * 50 + 1e6 / t) / (50 + 1);
    std::cout << DELETE << fps << " fps" << std::endl;

    // wait for delay to limit fps
    if (t < m_delay)
        SDL_Delay((m_delay - t) / 1000);
}

#endif
