#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
struct PointLight {
    public:
        Vector direction;
        Coeffecient intensity;
};

#endif
