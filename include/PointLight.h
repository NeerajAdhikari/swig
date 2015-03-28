#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Coeffecient.h"
#include "mathematics/Vector.h"
#include "Material.h"
#include "Camera.h"
#include "ScreenPoint.h"
#include "mathematics/Matrix.h"
#include "Drawer.h"
#include "TfMatrix.h"

class Shader;

struct PointLight {

public:
    Camera cam;
    Coeffecient intensity;
    double* shadow_buffer;
    Pair<unsigned> dim;
    Matrix<float> shadow_xForm;

    PointLight(Camera c, Coeffecient in) : cam(c), intensity(in),
        dim({0,0}), shadow_buffer(NULL), shadow_xForm({1,1})
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
    void updateShadowBuffer(Shader* sh, Plotter_* fb);
    void shFill(ScreenPoint a, ScreenPoint b, ScreenPoint c);
    void hLineD(int y, int xStart, int dStart, int xEnd, int dEnd,
            bool overwrite=false);
    int depthAt(int x, int y);
    bool onShadow(const Vector& pt);
};

inline bool PointLight::onShadow(const Vector& pt) {
    Matrix<float> shPt({4,1}); shPt(0,0)=pt.x; shPt(1,0)=pt.y;
    shPt(2,0)=pt.z; shPt(3,0)=pt.w;
    Vector v1;
    shPt /= shadow_xForm;
    Vector v = {shPt(0,0),shPt(1,0),shPt(2,0),shPt(3,0)};
    v.projectionNormalize();
    //v.display();
    //std::cout<<depthAt(v.x,v.y)<<std::endl;
    return depthAt(Math::round(v.x),Math::round(v.y))>
        (Math::round(v.z)+0.0008*INT32_MAX);
}

inline int PointLight::depthAt(int x, int y) {
    if (x>=dim.x || y>=dim.y || x<0 || y<0 )
        return 0;
    return shadow_buffer[y*dim.x+x];
}

#endif
