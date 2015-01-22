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

int main(int argc, char* argv[]) {
    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }


    // Initialize constant parameters
    const unsigned width = 800;
    const unsigned height = 600;

    const uintmax_t FPS = 100;
    const uintmax_t DELAY = 1e6/FPS;

    const float Iambient = 2;


    // TODO: Suface is inherited so that
    // color info and show can be kept inside it
    // even though it's only useful for flat shading and backface resp.
    // And all these light information and surface information should
    // be modeled
    // Surface parameters
    const float ka = .03;
    const float kd = .03;
    const float ks = .50;
    const float ns = 100;

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);

    // Intialize the benchmark
    Benchmark timekeeper;

    // Initialize the object
    Object tho(argv[1]);
    unsigned nSurfs = tho.surfaceCount();
    unsigned nVerts = tho.vertexCount();

    // Initialize projection matrix
    Matrix<float> proj = TfMatrix::perspective2(95,(float)width/height,10000,5);

    // Intialize the light sources
    std::vector<PointLight> light;
    {
        PointLight t = {{-1,-1,-1,0},10};
        PointLight m = {{1,0,0,0},10};
        light.push_back(t);
        light.push_back(m);
    }

    // For flat shading, the colors we need to fill surfaces with
    Color colors[nSurfs];
    // Detect backfaces - should we display the surface?
    bool show[nSurfs];

    // Intialize a transformation matrix to transform object
    const float ztranslate = -10;
    Matrix<float> rotator =
        TfMatrix::translation({ztranslate/2,0,ztranslate})
        * TfMatrix::rotation(2,{1,1,0},{0,0,0})
        * TfMatrix::translation({-ztranslate/2,0,-ztranslate});

    // Initially translate object to viewable part of
    // world coordinate
    tho.vmatrix() /= TfMatrix::translation({ztranslate/2,0,ztranslate});

    std::cout << "Ambient light intensity\t" << Iambient << std::endl;
    std::cout << "FPS limit:\t" << FPS << "\n" << std::endl;

    while (!fb.checkTerm()) {
        // Start benchmark time
        timekeeper.start();

        // Apply transformation to object
        tho.vmatrix() /= rotator;

        // Flat-shading : calculate the colors to shade each surface with
        for (int i=0; i<nSurfs; i++) {
            const Vector& normal = tho.getSurfaceNormal(i);
            const Vector& centroid = tho.getSurfaceCentroid(i);

            // Backface detection
            // TODO some problem in backface detection
            // backface detection for perspective view required
            /*
               if( normal.z < 0){
               show[i] = false;
               continue;
            // NOTE: for surfaces like paper
            // show[i] = true
            // normal.z += -1;
            } else {
            show[i] = true;
            }
            */
            show[i] = true;

            // Calculate ambient, diffused and specular lighting
            float intensity = Iambient*ka;
            for(int i=0; i<light.size(); i++){
                // Diffused lighting
                float cosine = ( (light[i].direction*-1) % normal);
                // This is to be done so that there won't be symmetric lighting
                if(cosine <= 0)
                    continue;

                Vector half = (light[i].direction + centroid)*(-1);
                // Specular ligting
                // TODO: In triangular surfaces in same plane
                // specular reflection doesn't give good result
                // for flat shading
                float cosineNs = std::pow( half.normalized() % normal , ns );

                // Get total intensity
                intensity += light[i].intensity*(kd*cosine + ks*cosineNs);
            }
            Uint8 clr = Math::min(intensity,1.0f) * 255;
            colors[i] =  {clr,clr,clr,255};
        }

        // Create a copy of object and apply the projection transform
        Object th = tho;
        th.vmatrix() /= proj;

        // Change the homogenous co-ordinates to normalized form
        // and device co-ordinate
        for (unsigned i=0; i<nVerts; i++) {
            // Perspective divide
            th(0,i) /= th(3,i);
            th(1,i) /= th(3,i);
            th(2,i) /= th(3,i);

            // Change the normalized X Y co-ordinates to device co-ordinate
            th(0,i) = th(0,i)*width + width/2;
            th(1,i) = height - (th(1,i)*height + height/2);
            // Converting normalized Z co-ordinate [-1,1] to viewport [1,0]*maxdepth
            th(2,i) = (-th(2,i)*0.5 + 0.5)*0xffff;
            // The Z map is like this
            //        0       n       f
            // -ve   inf      1       0   -ve
            //                 visible
        }

        // Clear framebuffer, we're about to plot
        drawer.clear();

        // Fill the surfaces
        for (int i=0; i<nSurfs; i++) {
            if(!show[i])
                continue;
            drawer.fillD(
                    th.getVertex(th.getSurface(i).x),
                    th.getVertex(th.getSurface(i).y),
                    th.getVertex(th.getSurface(i).z),
                    colors[i]);
        }

        // Update framebuffer
        drawer.update();

        // Stop benchmark time
        timekeeper.stop();
        uintmax_t ti = timekeeper.time();
        float real_fps = 1e6 / ti;
        // Convert us to ms and delay
        if( ti < DELAY){
            SDL_Delay((DELAY-ti)/1000);
        }

        std::cout << DELETE;
        std::cout << "Nolimit FPS:\t" << real_fps << std::endl;
    }
    return 0;
}
