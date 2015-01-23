#ifndef __POINT__
#define __POINT__
#include <stdint.h>
#include "Vector.h"
#include "helper.h"

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

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

    ScreenPoint() {
    }

    // Constructing ScreenPoint using Vector
    ScreenPoint(const Vector& v){
        x = Math::round(v.x);
        y = Math::round(v.y);
        d = Math::round(v.z);
    }
};

#endif
