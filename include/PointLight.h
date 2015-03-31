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
    double magic;

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
    void hLineD(int y, int xStart, int dStart, int xEnd, int dEnd);
    int depthAt(int x, int y);
    bool onShadow(const Vector& pt);
};

inline bool PointLight::onShadow(const Vector& pt) {
    // Using Math::round to round the values before checking
    // decreases FPS by a bit
    // return depthAt(Math::round(pt.x/pt.w),Math::round(pt.y/pt.w))
    //     >(Math::round(pt.z/pt.w)+magic*INT32_MAX);
    return depthAt((pt.x/pt.w),(pt.y/pt.w))
        >((pt.z/pt.w)+magic*INT32_MAX);
}

inline int PointLight::depthAt(int x, int y) {
    if (x>=dim.x || y>=dim.y || x<0 || y<0 )
        return 0;
    return shadow_buffer[y*dim.x+x];
}

inline void PointLight::hLineD(int y, int xStart, int dStart,
        int xEnd, int dEnd) {

    // Sort the start end end values if they are not in order

    if (xStart>xEnd) {
        swap(xStart,xEnd);
        swap(dStart,dEnd);
    }

    // If y lies outside then return
    if( y >= (int)dim.y || y < 0)
        return;
    // If x lies outside then return
    if( xStart >= (int)dim.x || xEnd < 0)
        return;

    Linspace d(dStart,dEnd,xStart,xEnd);

    // Clipping
    xEnd = Math::min(xEnd,(int)dim.x-1);
    // Clipping
    xStart = Math::max(0,xStart);

    while(xStart <= xEnd){
        // Depth clipping, checking with zero isn't necessary
        // as depth(xStart,y) is always greater than or equal
        // to 0
        // checking with far value must be done however
        // 0xffffff value because it is the maximum value it
        // should attain
        int de = d.at(xStart);
        if (de<=ScreenPoint::maxDepth && de>depthAt(xStart,y)) {
            shadow_buffer[y*dim.x+xStart] = de;
        }
        ++xStart;
    }
}

inline void PointLight::shFill(ScreenPoint a, ScreenPoint b,
        ScreenPoint c) {

    ScreenPoint start, mid, end;
    Drawer::initAscending(start,mid,end,a,b,c);

    if(start.y == end.y)
        return;
    if( start.y >= (int)dim.y || end.y < 0)
        return;
    // THe negative region is backside of the camera
    // or away from the far point
    if(start.d <= 0 || end.d <= 0 || mid.d <= 0)
        return;

    Linspace x1(start.x,mid.x, start.y, mid.y);
    Linspace x2(start.x,end.x, start.y, end.y);
    Linspace x3(mid.x,end.x, mid.y, end.y);

    Linspace d1(start.d,mid.d, start.y , mid.y);
    Linspace d2(start.d,end.d, start.y, end.y);
    Linspace d3(mid.d,end.d, mid.y, end.y);

    start.y = Math::min(mid.y,Math::max(start.y,0));
    for(int i=start.y;i<Math::min((int)dim.y,mid.y);i++)
        hLineD(i,x1.at(i),d1.at(i),x2.at(i),d2.at(i));

    // Clipping
    mid.y = Math::max(mid.y,0);
    for(int i=mid.y;i<=Math::min((int)dim.y-1,end.y);i++)
        hLineD(i, x2.at(i), d2.at(i), x3.at(i), d3.at(i));
}

#endif
