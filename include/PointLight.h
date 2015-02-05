#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
struct PointLight {
    public:
        Vector position;
        Coeffecient intensity;

    Coeffecient intensityAt(const Vector& pos){
        float ratio = (std::pow((pos-position).magnitude(),1) + 1);
        return {intensity.b/ratio, intensity.g/ratio, intensity.r/ratio};
    }

    Vector directionAt(const Vector& vec){
        return vec - position;
    }
};

#endif
