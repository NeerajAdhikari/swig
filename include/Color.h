#ifndef __COLOR__
#define __COLOR__

#include <stdint.h>
#include <iostream>

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;

    void display(){
        std::cout << (int)blue << " " << (int)green << " " << (int)red <<std::endl;
    }
};

#endif
