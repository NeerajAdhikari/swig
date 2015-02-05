#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
struct PointLight {
    public:
        Vector position;
        Coeffecient intensity;

    Coeffecient intensityAt(const Vector& pos){
        float ratio = (0.001*std::pow((pos-position).magnitude(),2) + 100);
        return {intensity.b/ratio, intensity.g/ratio, intensity.r/ratio};
    }

    Vector directionAt(const Vector& vec){
        return vec - position;
    }
};

#endif
