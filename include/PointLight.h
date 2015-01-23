#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "ex.h"
#include "helper.h"

// TODO add intensity of different color
struct PointLight {
    public:
    Vector direction;
    float intensity;

};

// TODO add intensity of different color
struct AmbientLight {
    public:
    float intensity;
};

#endif
