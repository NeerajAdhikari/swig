#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
#include "Material.h"

struct PointLight {
public:
    Vector position;
    Coeffecient intensity;

    Coeffecient intensityAt(const Vector& pos) {
        float ratio = (0.001*std::pow((pos-position).magnitude(),2) + 100);
        return {intensity.b/ratio, intensity.g/ratio, intensity.r/ratio};
    }

    Vector directionAt(const Vector& vec) {
        return vec - position;
    }

    Coeffecient lightingAt( const Vector& position, const Vector& normal, const Material& material, const Vector& vrp) {
        Coeffecient intensity;

        Coeffecient decintensity= intensityAt(position);
        Vector decdirection = directionAt(position);
        // Diffused lighting
        {
            float cosine = Vector::cosine((decdirection*(-1)),normal);
            // Check to remove symmetric lighting
            if(cosine > 0)
                intensity += (decintensity * material.kd) * cosine;
        }
        // Specular ligting
        {
            Vector half = (decdirection.normalized() + (position-vrp).normalized())*(-1);
            float cosine = Vector::cosine(half,normal);
            // Check to remove symmetric lighting
            if(cosine > 0)
                intensity += (decintensity * material.ks) * std::pow(cosine, material.ns);
        }

        return intensity;
    }
};

#endif

