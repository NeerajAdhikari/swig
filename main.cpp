#include "Drawer.h"
#include "Object.h"
#include "Matrix.h"
#include "Point.h"
#include "Vector.h"
#include "TfMatrix.h"
#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    // Initialize the plotter interface
    Plotter_ fb(800,600);
    Drawer drawer(&fb);

    // Initialize cam
    Matrix<float> cam({4,4});
    cam.initialize(
            10, 0, 0, 0,
            0, 10, 0, 0,
            0, 0,  0, 1,
            0, 0,  1, 0
            );

    // Initialize projection matrix
    // translate objects to the camera co-ordinates
    Matrix<float> proj = TfMatrix::translation({0,-8,0});
    // rotate objects to match the camera's orientation
    proj /= TfMatrix::rotation(270,{1,0,0},{0,0,0});
    // And then we need a perspective projection transform matrix
    proj /= cam;

    // Initialize the light source
    Vector light = {0,-1,0};
    light = light.normalized();

    // Initialize the object
    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount();
    unsigned nVerts = tho.vertexCount();

    // Intialize a transformation matrix to transform object
    Matrix<float> rotator = TfMatrix::rotation(2,{1,1,1},{0,0,0});

    // For flat shading, the colors we need to fill surfaces with
    // TODO: Suface is inherited so that
    // color info and show can be kept inside it
    // even though it's only useful for flat shading and backface resp.
    Color colors[nSurfs];

    // Detect backfaces - should we display the surface?
    //bool show[nSurfs];

    while (true) {

    // Change the world co-ordinates
    // Apply the rotation
    tho.vertex() /= rotator;

    // Flat-shading : calculate the colors to shade each surface with
    for (int i=0; i<nSurfs; i++) {
        Vector normal = tho.getSurfaceNormal(i);
        Vector light = {0,-1,0};
        float dp = (light%normal);
        //show[i]=(dp<-0.20);
        //show[i]=(dp<-0.01);
        //if (!show[i])
        //    continue;
        dp = fabs(dp);
        Uint8 clr = dp >= 1.0 ? 255 : dp*255;
        colors[i] =  {clr,clr,clr,255};
    }

    // Create a copy of object and apply the projection transform
    Object th = tho;
    th.vertex() /= proj;

    // The screen-points that our world-points will be mapped to
    ScreenPoint s[nVerts];
    // For each vertex,perform adjustments and calculate screen-points
    for (auto i=0; i<nVerts; i++) {
        Vector vert = th.getVertex(i);
        // Calculate depth
        s[i].d = 0xffffff/vert.w;
        // Normalize, to display on a 4by4 viewport
        vert.x/=vert.w*30;
        vert.y/=vert.w*30;
        if (vert.x<1.0)
            s[i].x = vert.x*800+400;
        if (vert.y<1.0)
            s[i].y = (-vert.y*600)+300;
    }

    // Clear framebuffer, we're about to plot
    drawer.clear();
    // Do the thing
    for (int i=0; i<nSurfs; i++) {
        //fb.line(s[th.getEdge(i).x],s[th.getEdge(i).y]);
        //if (show[i])
        drawer.fill(s[th.getSurface(i).x],s[th.getSurface(i).y],
                s[th.getSurface(i).z],colors[i]);
    }

    // Update framebuffer
    drawer.update();
    // Small delay to control framerate
    SDL_Delay(10);
    if (fb.checkTerm())
        break;
    }
}
