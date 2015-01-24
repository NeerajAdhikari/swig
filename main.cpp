#include "Matrix.h"
#include "TfMatrix.h"
#include "Vector.h"
#include "Point.h"
#include "Object.h"
#include "Drawer.h"
#include "Benchmark.h"
#include "lightandcolor.h"
#include <iostream>
#include "Linspace.h"
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

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);

    drawer.clear();

    // Intialize the benchmark
    Benchmark timekeeper;

    // Intialize the light sources
    AmbientLight ambient = {{2,2,2}};
    std::vector<PointLight> light;;
    {
        PointLight t = {{-1,-1,-1,0},{10,5,8}};
        PointLight m = {{1,0,0,0},{8,10,5}};
        light.push_back(t);
        light.push_back(m);
    }

    // Initialize the object
    Object obj(argv[1]);
    obj.initNormal();
    // material has a default constructor
    //obj.material = {0.01,0.05,0.05,270};
    unsigned nSurfs = obj.surfaceCount();
    unsigned nVerts = obj.vertexCount();

    // Initialize projection matrix
    Matrix<float> proj = TfMatrix::perspective2(95,(float)width/height,10000,5);

    // Intialize a transformation matrix to transform object
    const float ztranslate = -15;
    Matrix<float> rotator =
        TfMatrix::translation({0,0,ztranslate})
        * TfMatrix::rotation(2,{1,1,0},{0,0,0})
        * TfMatrix::translation({0,0,-ztranslate});

    // Initially translate object to viewable part of
    // world coordinate
    obj.vmatrix() /= TfMatrix::translation({0,0,ztranslate});

    // For flat shading, the colors we need to fill surfaces with
    Color colors[nVerts];
    // Detect backfaces - should we display the surface?
    //bool show[nSurfs];

    std::cout << "FPS limit:\t" << FPS << "\n" << std::endl;
    while (!fb.checkTerm()) {
        // Start benchmark time
        timekeeper.start();

        // Apply transformation to object
        obj.vmatrix() /= rotator;
        obj.nmatrix() /= TfMatrix::rotation(2,{1,1,0},{0,0,0});

        // Flat-shading : calculate the colors to shade each surface with

        for(auto i=0;i<nVerts;i++){

            Vector normal(obj.nmatrix()(0,i),obj.nmatrix()(1,i),obj.nmatrix()(2,i),obj.nmatrix()(3,i));
            Vector centroid(obj.vmatrix()(0,i),obj.vmatrix()(1,i),obj.vmatrix()(2,i),obj.vmatrix()(3,i));
            //const Vector& centroid = obj.getSurfaceCentroid(i);

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
            // if(!show[i])
            //    continue;
            unsigned a = copy.getSurface(i).x;
            unsigned b = copy.getSurface(i).y;
            unsigned c = copy.getSurface(i).z;

            drawer.fillD(
                    copy.getVertex(a),
                    copy.getVertex(b),
                    copy.getVertex(c),
                    colors[a],colors[b],colors[c]);
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
