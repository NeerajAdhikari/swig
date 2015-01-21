#include "Matrix.h"
#include "TfMatrix.h"
#include "Vector.h"
#include "Point.h"
#include "Object.h"
#include "Drawer.h"
#include <iostream>
#include <cmath>


const unsigned width = 800;
const unsigned height = 600;
const float ambient_light = 0.2;

int main(int argc, char* argv[]) {

    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);

    // Initialize projection matrix
    Matrix<float> proj = TfMatrix::perspective2(95,(float)width/height,10000,5);

    // Initialize the direction of light source
    Vector light = {-1,-1,-1, 0};
    light = light.normalized();

    // Just a temporary variable for debugging
    const float ztranslate = -20;

    // Initialize the object
    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount();
    unsigned nVerts = tho.vertexCount();

    // Translate the object to some place visible on screen
    tho.vmatrix() /= TfMatrix::translation({0,0,ztranslate});

    // Intialize a transformation matrix to transform object
    Matrix<float> rotator = TfMatrix::translation({0,0,ztranslate})
        * TfMatrix::rotation(2,{1,1,0},{0,0,0})
        * TfMatrix::translation({0,0,-ztranslate});

    // TODO: Suface is inherited so that
    // color info and show can be kept inside it
    // even though it's only useful for flat shading and backface resp.

    // For flat shading, the colors we need to fill surfaces with
    Color colors[nSurfs];

    // Detect backfaces - should we display the surface?
    //bool show[nSurfs];

    while (true) {

        // Apply transformation to object
        tho.vmatrix() /= rotator;

        // Flat-shading : calculate the colors to shade each surface with
        for (int i=0; i<nSurfs; i++) {

            Vector normal = tho.getSurfaceNormal(i);
            // UPDATE
            // To be done if a surface is to be displayed
            // from both sides like a thin paper
            // TODO doesn't work very nice
            /*
            if( normal.z < 0)
                normal *= -1;
            */

            float cosine = (light%normal);
            //show[i]=(cosine<-0.20);
            //if (!show[i])
            //    continue;

            Uint8 intensity = 255*ambient_light;
            // UPDATE
            // This is to be done so that there won't be symmetric lighting
            if(cosine < 0){
                // make cosine positive
                cosine *= -1;
                intensity += (1-ambient_light)*(cosine>1?1:cosine)*255;
            }
            colors[i] =  {intensity,intensity,intensity,255};
        }

        // Create a copy of object and apply the projection transform
        Object th = tho;
        th.vmatrix() /= proj;

        // The screen-points that our world-points will be mapped to
        //ScreenPoint s[nVerts];
        // For each vertex,perform adjustments and calculate screen-points

        // TODO clipping
        for (auto i=0; i<nVerts; i++) {

            // Perspective divide
            th(0,i) /= th(3,i);
            th(1,i) /= th(3,i);
            th(2,i) /= th(3,i);

            // Change the co-ordinates to device co-ordinate
            th(0,i) = th(0,i)*width + width/2;
            th(1,i) = height - (th(1,i)*height + height/2);
            // Converting Normalized Z co-ordinate [-1,1] to viewport depth [1,0]*maxdepth
            th(2,i) = (-th(2,i)*0.5 + 0.5)*0xffffff;
        }

        // Clear framebuffer, we're about to plot
        drawer.clear();

        // Do the thing
        for (int i=0; i<nSurfs; i++) {
            //fb.line(s[th.getEdge(i).x],s[th.getEdge(i).y]);
            //if (show[i])
            drawer.fillD(
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
        if (fb.checkTerm())
            break;
        else
            SDL_Delay(10);
    }
}
