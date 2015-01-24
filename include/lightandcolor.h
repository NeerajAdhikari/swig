#ifndef __LIGHT_AND_COLOR__
#define __LIGHT_AND_COLOR__

#include "ex.h"
#include "helper.h"
#include "Vector.h"

// RGB coeffecient
struct Coeffecient {
    public:
        float r,g,b;
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
    // TODO constructor
};

#endif
