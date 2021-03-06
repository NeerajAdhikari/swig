#ifndef __SCREENPOINT__
#define __SCREENPOINT__

#include <stdint.h>
#include "mathematics/Vector.h"
#include "Color.h"

struct ScreenPoint {
    const static int32_t maxDepth = INT32_MAX;
    // Changed int16 to int32
    // ScreenPoint needs to hold infromation about
    // points outside of the Device co-ordinate
    // Ex: drawing a clipped line, triangle
    int32_t x;
    int32_t y;
    int32_t d;
    Color color;
    Vector real;

    ScreenPoint(){
    }

    ScreenPoint(const Vector& vec, const Color& col){
        x = Math::round(vec.x);
        y = Math::round(vec.y);
        d = Math::round(vec.z);
        color = col;
    }

    void print() {
        std::cout<<'('<<x<<','<<y<<','<<d<<')';
        color.print();
    }
};

#endif
