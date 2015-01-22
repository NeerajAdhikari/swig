#ifndef __BENCHMARK__
#define __BENCHMARK__
#include "ex.h"
#include "helper.h"
#include <iostream>
#include <sys/time.h>

class Benchmark {
    private:
        timeval m_start,m_end;
        bool m_running;
    public:
        Benchmark() : m_running(false) {
        }
        void start() {
            if(!m_running) {
                gettimeofday(&m_start,0);
                m_running = true;
            }
        }
        void stop() {
            if(m_running){
                gettimeofday(&m_end,0);
                m_running = false;
            }
        }
        // time in microseconds
        uintmax_t time() {
            if(m_running)
                throw "WTF";
            return (m_end.tv_sec * 1e6 + m_end.tv_usec - m_start.tv_sec * 1e6 - m_start.tv_usec);
        }
};

#endif
