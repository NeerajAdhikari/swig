#ifndef __BENCHMARK__
#define __BENCHMARK__
#include "ex.h"
#include "helper.h"
#include <sys/time.h>

class Time {
    private:
        timeval m_start,m_end;
        bool m_running;
    public:

        Time () : m_running(false) {
        }

        // Start the benchmark
        inline void start() {
            if(!m_running) {
                gettimeofday(&m_start,0);
                m_running = true;
            }
        }

        // Stop the benchmark
        inline void stop() {
            if(m_running){
                gettimeofday(&m_end,0);
                m_running = false;
            }
        }

        // Return the difference between start time
        // and stop time in micro seconds
        inline uintmax_t time() {
            if(m_running)
                throw ex::Wtf();
            return (m_end.tv_sec * 1e6 + m_end.tv_usec - m_start.tv_sec * 1e6 - m_start.tv_usec);
        }
};

#endif
