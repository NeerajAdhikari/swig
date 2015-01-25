#ifndef __SCREENPOINT__
#define __SCREENPOINT__

#include <stdint.h>
#include "Color.h"

struct ScreenPoint {
    // Changed int16 to int32
    // Changed int32 to int64
    // ScreenPoint needs to hold infromation about
    // points outside of the Device co-ordinate
    // Ex: drawing a clipped line, triangle
    int32_t x;
    int32_t y;
    int64_t d;
    Color color;
};

#endif
