#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "ex.h"
#include "helper.h"
#include "Vector.h"

struct PointLight {
    public:
    Vector direction;
    float r,g,b;
    // TODO constructor
};

struct AmbientLight {
    public:
    float r,g,b;
};

struct Coeffecient {
    public:
    float r,g,b;
    Coeffecient(float x, float y, float z):
        r(x), g(y), b(z)
    {
    }
};

#endif
