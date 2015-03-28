#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
#include "Material.h"
#include "Camera.h"
#include "ScreenPoint.h"

class Shader;

struct PointLight {

public:
    Camera cam;
    Coeffecient intensity;
    int* shadow_buffer;
    Pair<unsigned> dim;

    PointLight(Camera c, Coeffecient in) : cam(c), intensity(in),
        dim({0,0})
    {}

    Coeffecient intensityAt(const Vector& pos) {
        float ratio = (0.001*std::pow((pos-cam.vrp)
                    .magnitude(),2) + 100);
        return {intensity.b/ratio, intensity.g/ratio,
            intensity.r/ratio};
    }

    Vector directionAt(const Vector& vec) {
        return vec - cam.vrp;
    }

    Coeffecient lightingAt( const Vector& position,
            const Vector& normal, const Material& material,
            const Vector& vrp) {

        Coeffecient intensity;

        Coeffecient decintensity= intensityAt(position);
        Vector decdirection = directionAt(position);
        // Diffused lighting
        {
            float cosine = Vector::cosine((decdirection*(-1)),
                    normal);
            // Check to remove symmetric lighting
            if(cosine > 0)
                intensity += (decintensity*material.kd)*cosine;
        }
        // Specular ligting
        {
            Vector half = (decdirection.normalized()+(
                        position-vrp).normalized())*(-1);
            float cosine = Vector::cosine(half,normal);
            // Check to remove symmetric lighting
            if(cosine > 0)
                intensity += (decintensity*material.ks)*
                    std::pow(cosine,material.ns);
        }

        return intensity;
    }

    void initShadowBuffer(Pair<unsigned> dim);
    void updateShadowBuffer(Shader* sh);
    void shFill(ScreenPoint a, ScreenPoint b, ScreenPoint c);
    void hLineD(int y, int xStart, int dStart, int xEnd, int dEnd,
            bool overwrite=false);
    int depthAt(unsigned x, unsigned y);
    bool onShadow(const Vector& pt);
};

#endif

