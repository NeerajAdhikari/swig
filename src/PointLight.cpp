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
    shadow_xForm =
        TfMatrix::toDevice(dim.x,dim.y,ScreenPoint::maxDepth)
        *TfMatrix::perspective(95,((float)dim.y)/(float)dim.x
                ,500,5)
        *TfMatrix::lookAt(cam.vrp,cam.vpn,cam.vup);
}

void PointLight::updateShadowBuffer(Shader* sh, Plotter_* fb) {
    memset((void*)shadow_buffer,0,(sizeof (double))*(dim.x*dim.y));
    for (int k=0; k<sh->objectCount(); k++) {
        Object obj = *(sh->getObjectP(k));
        Matrix<float>& vAlias = obj.vcmatrix();
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
            if (obj.getSurfaceNormal(i)%cam.vpn>0)
                continue;
            ScreenPoint a(obj.getCopyVertex(index),black);
            index = obj.getSurface(i).y;
            ScreenPoint b(obj.getCopyVertex(index),black);
            index = obj.getSurface(i).z;
            ScreenPoint c(obj.getCopyVertex(index),black);
            shFill(a,b,c);
        }
    }
}

