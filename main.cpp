#include "Drawer.h"
#include "Object.h"
#include "Matrix.h"
#include "Point.h"
#include "structures.h"
#include "VectorTriplet.h"
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

    // A matrix to perform a 5degree rotation about z-axis
    Matrix rotator = TfMatrix::rotation(5,{0,0,1},{0,0,0});
    //rotator = rotator*TfMatrix::rotation(2,{0,1,0},{0,0,0});
    rotator = rotator*TfMatrix::rotation(2,{1,0,0},{0,0,0});

    // Lets start building the projection matrix.
    // First we need to translate objects to the camera co-ordinates
    Matrix proj = TfMatrix::translation({0,-400,0});
    // Then rotate to match the camera's orientation
    proj = TfMatrix::rotation(270,{1,0,0},{0,0,0})*proj;
    // And then we need a perspective projection transform matrix
    Matrix cam = Matrix::identity(4);
    // Project on z=3
    cam(0,0) = 10; cam(1,1) = 10; cam(2,2) = 0; cam(3,3) = 0;
    cam(3,2) = 1; cam(2,3) = 1;
    proj = cam*proj;

    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount(), nVerts = tho.vertexCount();

    // For the colors we need to fill surfaces with
    Color colors[nSurfs];
    // Detect backfaces - shoudl we display the surface?
    bool show[nSurfs];

    while (true) {

    // Flat-shading : calculate the colors to shade each surface with
    for (int i=0; i<nSurfs; i++) {
        VectorTriplet normal = tho.getSurfaceNormal(i);
        VectorTriplet light = {0,-1,0};
        float dp = (light%normal);
        //show[i]=(dp<-0.20);
        //show[i]=(dp<-0.01);
        //if (!show[i])
        //    continue;
        Uint8 clr;
        if (fabs(dp)>=1.0)
            clr=255;
        else
            clr=(Uint8)(fabs(dp)*255.0);
        colors[i] =  {clr,clr,clr,255};
    }
    // Apply the rotation
    tho = rotator*tho;

    // Create a copy and apply the projection transform
    Object th = tho;
    th=proj*tho;

    // The screen-points that our world-points will be mapped to
    ScreenPoint s[nVerts];

    // For each vertex,perform adjustments and calculate screen-points
    for (auto i=0; i<nVerts; i++) {
        VectorTriplet vert = th.getVertex(i);
        // Calculate depth
        s[i].d = 0xffffff*vert.z;
        // Normalize, to display on a 4by4 viewport
        vert.x/=30; vert.y/=30;
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
    SDL_Delay(20);

    if (fb.checkTerm())
        break;
    }
}
