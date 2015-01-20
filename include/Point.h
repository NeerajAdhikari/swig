#ifndef __POINT__
#define __POINT__

#include <stdint.h>

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

struct ScreenPoint {
    Color color;
    uint16_t x;
    uint16_t y;
    uint32_t d;
};

#endif
