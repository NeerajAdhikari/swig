#ifndef __POINT__
#define __POINT__

#include <stdint.h>
#include "Vector.h"
#include "helper.h"
#include "ex.h"

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
};



// RGB coeffecient
struct Coeffecient {
    public:
        float b,g,r;
};

struct PointLight {
    public:
        Vector direction;
        Coeffecient intensity;
};

struct AmbientLight {
    public:
        Coeffecient intensity;
};

struct Material {
    // ambient-reflection coefficient
    Coeffecient ka;
    // diffuse-reflection coefficient
    Coeffecient kd;
    // specular-reflection coefficient
    Coeffecient ks;
    // specular-reflection parameter (smaller values for dull surfaces)
    float ns;

    Material():
        ka({0.01,0.01,0.01}),
        kd({0.05,0.05,0.05}),
        ks({0.05,0.05,0.05}),
        ns(10)
    {
    }
};



#endif
