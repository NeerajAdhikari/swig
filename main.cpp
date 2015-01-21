#include "Matrix.h"
#include "TfMatrix.h"
#include "Vector.h"
#include "Point.h"
#include "Object.h"
#include "Drawer.h"
#include <iostream>
#include <cmath>

// TODO
// UPDATE:    Vertex isn't divided by "w" through getVertex method

int main(int argc, char* argv[]) {
    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }
    const unsigned width = 400;
    const unsigned height = 400;

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);

    /*
       Matrix<float> cam({4,4});
       cam.initialize(
       10, 0, 0, 0,
       0, 10, 0, 0,
       0, 0,  0, 1,
       0, 0,  1, 0
       );
       */

    // Initialize projection matrix
    // translate objects to the camera co-ordinates
    Matrix<float> proj = TfMatrix::translation({0,0,-10});
    // rotate objects to match the camera's orientation
    proj /= TfMatrix::rotation(180,{0,1,0},{0,0,0});
    // And then we need a perspective projection transform matrix
    proj /= TfMatrix::perspective2(95,1.0*width/height,1000,10);

    // Initialize the light source
    Vector light = {0,-1,-1, 0};
    light = light.normalized();

    // Initialize the object
    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount();
    unsigned nVerts = tho.vertexCount();

    // Intialize a transformation matrix to transform object
    Matrix<float> rotator = TfMatrix::rotation(2,{1,1,0},{0,0,0});

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
        //ScreenPoint s[nVerts];
        // For each vertex,perform adjustments and calculate screen-points

        for (auto i=0; i<nVerts; i++) {
            /*
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
            */

            // TODO clipping

            // Perspective divide
            th.vertex()(0,i) /= th.vertex()(3,i);
            th.vertex()(1,i) /= th.vertex()(3,i);
            th.vertex()(2,i) /= th.vertex()(3,i);

            //th.vertex()(2,i) = 0xffffff / th.vertex()(3,i);
            th.vertex()(0,i) = th.vertex()(0,i)*width + width/2;
            th.vertex()(1,i) = th.vertex()(1,i)*height + height/2;
            // Converting Normalized Z [-1,1] to viewport depth [0,1]*maxdepth
            th.vertex()(2,i) = (th.vertex()(2,i)*0.5 + 0.5)*0xffffff;

        }


        // Clear framebuffer, we're about to plot
        drawer.clear();
        // Do the thing
        for (int i=0; i<nSurfs; i++) {
            //fb.line(s[th.getEdge(i).x],s[th.getEdge(i).y]);
            //if (show[i])
            drawer.fill(
                    th.getVertex(th.getSurface(i).x),
                    th.getVertex(th.getSurface(i).y),
                    th.getVertex(th.getSurface(i).z),
                    colors[i]);
            /*
               drawer.fill(s[th.getSurface(i).x],s[th.getSurface(i).y],
               s[th.getSurface(i).z],colors[i]);
               */
        }

        // Update framebuffer
        drawer.update();
        // Small delay to control framerate
        SDL_Delay(10);
        if (fb.checkTerm())
            break;
    }
}
