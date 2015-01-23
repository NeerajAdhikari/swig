#include "Matrix.h"
#include "TfMatrix.h"
#include "Vector.h"
#include "Point.h"
#include "Object.h"
#include "Drawer.h"
#include "Benchmark.h"
#include "lightandcolor.h"
#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }


    // Initialize constant parameters
    const unsigned width = 400;
    const unsigned height = 300;

    const uintmax_t FPS = 100;
    const uintmax_t DELAY = 1e6/FPS;

    // TODO: Suface is inherited so that
    // color info and show can be kept inside it
    // even though it's only useful for flat shading and backface resp.
    // And all these light information and surface information should
    // be modeled

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);

    // Intialize the benchmark
    Benchmark timekeeper;

    // Initialize the object
    Object obj(argv[1]);
    unsigned nSurfs = obj.surfaceCount();
    unsigned nVerts = obj.vertexCount();
    //obj.material = {0.01,0.05,0.05,270};

    // Initialize projection matrix
    Matrix<float> proj = TfMatrix::perspective2(95,(float)width/height,10000,5);

    // Intialize the light sources
    std::vector<PointLight> light;
    {
        PointLight t = {{-1,-1,-1,0},{10,0,0}};
        PointLight m = {{1,0,0,0},{0,10,0}};
        light.push_back(t);
        light.push_back(m);
    }
    AmbientLight ambient = {{2,2,2}};

    // For flat shading, the colors we need to fill surfaces with
    Color colors[nSurfs];
    // Detect backfaces - should we display the surface?
    bool show[nSurfs];

    // Intialize a transformation matrix to transform object
    const float ztranslate = -15;
    Matrix<float> rotator =
        TfMatrix::translation({0,0,ztranslate})
        * TfMatrix::rotation(2,{1,1,0},{0,0,0})
        * TfMatrix::translation({0,0,-ztranslate});

    // Initially translate object to viewable part of
    // world coordinate
    obj.vmatrix() /= TfMatrix::translation({0,0,ztranslate});

    //std::cout << "Ambient light intensity\t" << ambient.intensity << std::endl;
    std::cout << "FPS limit:\t" << FPS << "\n" << std::endl;


    while (!fb.checkTerm()) {
        // Start benchmark time
        timekeeper.start();

        // Apply transformation to object
        obj.vmatrix() /= rotator;

        // Flat-shading : calculate the colors to shade each surface with
        for (int i=0; i<nSurfs; i++) {
            const Vector& normal = obj.getSurfaceNormal(i);
            const Vector& centroid = obj.getSurfaceCentroid(i);

            show[i] = true;
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

            // Calculate ambient, diffused and specular lighting
            float intensityR = ambient.intensity.r*obj.material.ka.r;
            float intensityG = ambient.intensity.g*obj.material.ka.g;
            float intensityB = ambient.intensity.b*obj.material.ka.b;
            for(int i=0; i<light.size(); i++){
                // Diffused lighting
                float cosine = Vector::cosine((light[i].direction*(-1)),normal);
                // This is to be done so that there won't be symmetric lighting
                if(cosine > 0){
                    intensityR += light[i].intensity.r*obj.material.kd.r*cosine;
                    intensityG += light[i].intensity.g*obj.material.kd.g*cosine;
                    intensityB += light[i].intensity.b*obj.material.kd.b*cosine;
                } else
                    continue;

                // Specular ligting
                // TODO: In triangular surfaces in same plane
                // specular reflection doesn't give good result
                // for flat shading
                Vector half = (light[i].direction + centroid)*(-1);
                float cosineNs = std::pow( Vector::cosine(half,normal), obj.material.ns );

                if(cosineNs >0){
                    intensityR += light[i].intensity.r*obj.material.ks.r*cosineNs;
                    intensityG += light[i].intensity.g*obj.material.ks.g*cosineNs;
                    intensityB += light[i].intensity.b*obj.material.ks.b*cosineNs;
                }
            }
            Uint8 clrR = Math::min(intensityR,1.0f) * 255;
            Uint8 clrG = Math::min(intensityG,1.0f) * 255;
            Uint8 clrB = Math::min(intensityB,1.0f) * 255;
            colors[i] =  {clrR,clrG,clrB,255};
        }

        // Create a copy of object and apply the projection transform
        Object copy = obj;
        copy.vmatrix() /= proj;

        // Change the homogenous co-ordinates to normalized form
        // and device co-ordinate
        for (unsigned i=0; i<nVerts; i++) {
            // Perspective divide
            copy(0,i) /= copy(3,i);
            copy(1,i) /= copy(3,i);
            copy(2,i) /= copy(3,i);

            // Change the normalized X Y co-ordinates to device co-ordinate
            copy(0,i) = copy(0,i)*width + width/2;
            copy(1,i) = height - (copy(1,i)*height + height/2);
            // Converting normalized Z co-ordinate [-1,1] to viewport [1,0]*maxdepth
            copy(2,i) = (-copy(2,i)*0.5 + 0.5)*0xffff;
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
                    copy.getVertex(copy.getSurface(i).x),
                    copy.getVertex(copy.getSurface(i).y),
                    copy.getVertex(copy.getSurface(i).z),
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
