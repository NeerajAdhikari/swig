#include "SDLPlot.h"
#include "Object.h"
#include "Matrix.h"
#include "Point.h"
#include "structures.h"
#include "VectorTriplet.h"
#include "TfMatrix.h"
#include <iostream>
#include <cmath>

int main() {
    SDLPlotter fb;
    Matrix rotator = TfMatrix::rotation(5,{0,0,1},{0,0,0});
    Matrix proj = TfMatrix::translation({0,-4,0});
    proj = TfMatrix::rotation(270,{1,0,0},{0,0,0})*proj;
    Matrix cam = Matrix::identity(4);
    cam(0,0) = 3;
    cam(1,1) = 3;
    cam(2,2) = 3;
    cam(3,3) = 0;
    cam(3,2) = 1;
    proj = cam*proj;

    Object tho(4);
    tho.setVertex(0,{0,0,0});
    tho.setVertex(1,{1,0,0});
    tho.setVertex(2,{0,1,0});
    tho.setVertex(3,{0,0,1});
    tho.setEdge({0,1}); tho.setEdge({0,2}); tho.setEdge({0,3});
    tho.setEdge({1,2}); tho.setEdge({1,3}); tho.setEdge({2,3});
    tho.setSurface({0,1,3});
    tho.setSurface({0,2,1});
    tho.setSurface({0,3,2});
    tho.setSurface({1,2,3});

    Color colors[4];
    bool show[4];

    for (auto k=0;k<120;k++) {
    for (int i=0; i<4; i++) {
        VectorTriplet normal = tho.getSurfaceNormal(i);
        VectorTriplet light = {0,-1,0};
        float dp = (light%normal);
        show[i]=(dp<0);
        if (!show[i])
            continue;
        Uint8 clr;
        if (fabs(dp)>=1.0)
            clr=255;
        else
            clr=(Uint8)(fabs(dp)*255.0);
        colors[i] =  {clr,clr,clr,255};
    }
    tho = rotator*tho;

    Object th = tho;
    th=proj*tho;
    ScreenPoint s[4];
    for (auto i=0; i<4; i++) {
        VectorTriplet vert = th.getVertex(i);
        vert = vert.normalized();
        vert.x/=2; vert.y/=2; vert.z/=2;
        s[i].x = vert.x*800+400;
        s[i].y = (-vert.y*600)+300;
        s[i].color = colors[i];
    }

    fb.clear();
    for (int i=0; i<4; i++) {
        //fb.line(s[th.getEdge(i).x],s[th.getEdge(i).y]);
        if (show[i])
        fb.fill(s[th.getSurface(i).x],s[th.getSurface(i).y],
                s[th.getSurface(i).z],colors[i]);
    }
    fb.update();
    SDL_Delay(10);
    }
}
