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
    Matrix rotator = TfMatrix::rotation(1.3,{0,0,1},{0,0,0}) 
        * TfMatrix::rotation(1.2,{0,1,0},{0,0,0});
      //  * TfMatrix::rotation(1,{1,0,0},{0,0,0});

    // Lets start building the projection matrix.
    // First we need to translate objects to the camera co-ordinates
    Matrix proj = TfMatrix::translation({-1,-5,-0.2});
    // Then rotate to match the camera's orientation
    proj /= TfMatrix::rotation(270,{1,0,0},{0,0,0});

    // And then we need a perspective projection transform matrix
    // Project on z=3
    // Note: I can write a projection matrix
    // I don't know how it should be though
    Matrix cam = Matrix::identity(4);
    /*
    cam.initialize(
            2, 0, 0, 0,
            0, 2, 0, 0,
            0, 0, 2, 0,
            0, 0, 1, 0
            );
    proj /= cam;
    */
    cam(0,0) = 2;
    cam(1,1) = 2;
    cam(2,2) = 2;
    cam(3,3) = 0;
    cam(3,2) = 1;
    proj /= cam;

    /* An object with 4 vertices (a tetrahedron)
       Object tho(4);
    // Set the four points
    tho.setVertex(0,{0,0,0});
    tho.setVertex(1,{1,0,0});
    tho.setVertex(2,{0,1,0});
    tho.setVertex(3,{0,0,1});
    // Set the six edges
    tho.setEdge({0,1}); tho.setEdge({0,2}); tho.setEdge({0,3});
    tho.setEdge({1,2}); tho.setEdge({1,3}); tho.setEdge({2,3});
    // Then set the four surfaces
    tho.setSurface({0,1,3});
    tho.setSurface({0,2,1});
    tho.setSurface({0,3,2});
    tho.setSurface({1,2,3});
    */

    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount();
    unsigned nVerts = tho.vertexCount();

    // For the colors we need to fill surfaces with
    Color colors[nSurfs];
    // Detect backfaces - should we display the surface?
    bool show[nSurfs];

    // Flat-shading : calculate the colors to shade each surface with
    VectorTriplet light = {0,-1,0};
    light = light.normalized();

    while (true) {

        for (int i=0; i<nSurfs; i++) {
            VectorTriplet normal = tho.getSurfaceNormal(i);
            float dp = (light%normal);

            show[i]=(dp<-0.20);
            if (!show[i])
                continue;

            Uint8 clr;
            if (fabs(dp)>=1.0)
                clr = 255;
            else
                clr = (Uint8)(fabs(dp)*255.0);

            colors[i] =  {clr,clr,clr,255};
        }

        // Apply the rotation
        tho.vertex() /= rotator;

        // Create a copy and apply the projection transform
        Object th = tho;
        th.vertex() /= proj;

        // The screen-points that our world-points will be mapped to
        ScreenPoint s[nVerts];

        // For each vertex,perform adjustments and calculate screen-points
        for (auto i=0; i<nVerts; i++) {
            VectorTriplet vert = th.getVertex(i);
            vert = vert.normalized();
            vert.x/=2; vert.y/=2; vert.z/=2;
            s[i].x = vert.x*800+400;
            s[i].y = (-vert.y*600)+300;
            s[i].color = colors[i];
        }

        // Clear framebuffer, we're about to plot
        drawer.clear();

        // Do the thing
        for (int i=0; i<nSurfs; i++) {
            //fb.line(s[th.getEdge(i).x],s[th.getEdge(i).y]);
            if (show[i])
                drawer.fill(s[th.getSurface(i).x],s[th.getSurface(i).y],
                        s[th.getSurface(i).z],colors[i]);
        }

        // Update framebuffer
        drawer.update();

        // Small delay to control framerate
        //SDL_Delay(20);

        if (fb.checkTerm())
            break;
    }
}
