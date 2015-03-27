#ifndef __COLOR__
#define __COLOR__

#include <stdint.h>
#include <iostream>

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

// Set of standard colors
// I don't know why no extern is required here!
const static Color black = {0,0,0,255};
const static Color white = {255,255,255,255};
const static Color badcolor = {255,0,255,255};

#endif
