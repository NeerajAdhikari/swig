#include "Matrix.h"
#include "TfMatrix.h"
#include "Vector.h"
#include "Point.h"
#include "Object.h"
#include "Drawer.h"
#include "Benchmark.h"
#include "PointLight.h"
#include <iostream>
#include <cmath>


const unsigned width = 800;
const unsigned height = 600;

const uintmax_t FPS = 60;
const uintmax_t DELAY = 1e6/DELAY;

const float Iambient = 10;

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

    std::vector<PointLight> light;

    {
        PointLight t;

        t.direction = {-1,-1,-1,0};
        t.intensity = 10;
        light.push_back(t);

        /*
        t.direction = {1,0,0,0};
        t.intensity = 10;
        light.push_back(t);
        */
    }

    // Just a temporary variable for debugging
    const float ztranslate = -15;

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


    // For flat shading, the colors we need to fill surfaces with
    Color colors[nSurfs];

    // Detect backfaces - should we display the surface?
    //bool show[nSurfs];

    Benchmark timekeeper;
    while (true) {
        timekeeper.start();

        // Apply transformation to object
        tho.vmatrix() /= rotator;

        // Flat-shading : calculate the colors to shade each surface with
        for (int i=0; i<nSurfs; i++) {

            // TODO: Suface is inherited so that
            // color info and show can be kept inside it
            // even though it's only useful for flat shading and backface resp.
            // And all these light information and surface information should
            // be modeled
            const float ka = .007;
            const float kd = .060;
            const float ks = .090;
            const float ns = 10;

            float Il = 10;

            Vector normal = tho.getSurfaceNormal(i);
            Vector centroid = tho.getSurfaceCentroid(i);
            // UPDATE
            // To be done if a surface is to be displayed
            // from both sides like a thin paper
            // TODO doesn't work very nice
            /*
               if( normal.z < 0)
               normal *= -1;
               */


            float intensity = Iambient*ka;

            for(int i=0;i<light.size();i++){

                float cosine = ( (light[i].direction*-1) % normal);

                Vector half = (light[i].direction + centroid)*(-1);
                half = half.normalized();
                float cosineNs = std::pow( half % normal , ns );
                //show[i]=(cosine>0.20);
                //if (!show[i])
                //    continue;

                // UPDATE
                // This is to be done so that there won't be symmetric lighting
                if(cosine > 0){
                    // make cosine positive
                    intensity += light[i].intensity*(kd*cosine + ks*cosineNs);
                }

            }


            if( intensity > 1)
                intensity = 1;
            Uint8 clr = intensity * 255;

            colors[i] =  {clr,clr,clr,255};
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
            return 0;

        timekeeper.stop();
        uintmax_t ti = timekeeper.time();
        // float real_fps = 1e6 / ti;
        if( ti < DELAY)
            SDL_Delay((DELAY-ti)/1000);
    }
    return 0;
}
