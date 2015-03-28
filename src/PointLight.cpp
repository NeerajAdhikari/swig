#include "PointLight.h"
#include "Shader.h"
#include "Drawer.h"
#include "common/containers.h"
#include "TfMatrix.h"
#include "ScreenPoint.h"


void PointLight::initShadowBuffer(Pair<unsigned> dm) {
    dim = dm;
    shadow_buffer = new double[dim.x*dim.y];
    memset((void*)shadow_buffer,0,(sizeof (double))*(dim.x*dim.y));
}

void PointLight::updateShadowBuffer(Shader* sh, Plotter_* fb) {
    shadow_xForm =
        TfMatrix::toDevice(dim.x,dim.y,ScreenPoint::maxDepth)
        *TfMatrix::perspective(95,((float)dim.y)/(float)dim.x
                ,500,5)
        *TfMatrix::lookAt(cam.vrp,cam.vpn,cam.vup);

    for (int k=0; k<sh->objectCount(); k++) {
        Object obj = *(sh->getObjectP(k));
        Matrix<float>& vAlias = obj.vmatrix();
        vAlias /= shadow_xForm;

        // Perspective divide, homogenous co-ordinates
        // to normalized co-ordinate
        // NOTE: this can be done later in life
        for (unsigned i=0; i<obj.vertexCount(); i++) {
            vAlias(0,i) /= vAlias(3,i);
            vAlias(1,i) /= vAlias(3,i);
            vAlias(2,i) /= vAlias(3,i);
            vAlias(3,i) = 1.0;
        }

        for (auto i=0; i<obj.surfaceCount(); i++) {
            int index = obj.getSurface(i).x;
            ScreenPoint a(obj.getVertex(index),black);
            index = obj.getSurface(i).y;
            ScreenPoint b(obj.getVertex(index),black);
            index = obj.getSurface(i).z;
            ScreenPoint c(obj.getVertex(index),black);
            shFill(a,b,c);
        }
    }
}

void PointLight::shFill(ScreenPoint a, ScreenPoint b, ScreenPoint c) {

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

void PointLight::hLineD(int y, int xStart, int dStart,
        int xEnd, int dEnd, bool overwrite) {

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
        if ((overwrite && de<=ScreenPoint::maxDepth &&
                    de>=depthAt(xStart,y)) ||
            (!overwrite &&  de<=ScreenPoint::maxDepth &&
             de>depthAt(xStart,y)) ) {
            shadow_buffer[y*dim.x+xStart] = de;
        }
        ++xStart;
    }
}


