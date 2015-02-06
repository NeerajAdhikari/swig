#include <iostream>
#include <cstring>

#include "mathematics/Vector.h"
#include "mathematics/Matrix.h"
#include "TfMatrix.h"

#include "Drawer.h"

#include "PointLight.h"
#include "AmbientLight.h"
#include "Object.h"

#include "misc/Time.h"

// Initialize constant parameters
const uint16_t WIDTH = 800;
const uint16_t HEIGHT = 600;
const uintmax_t FPS = 100;
const uintmax_t DELAY = 1e6/FPS;
// Backface detection
const bool BACKFACEDETECTION = true;
//const bool UNBOUNDED = false;
// Gourad Shading
const bool GOURAD = true;

int main(int argc, char* argv[]) {

    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    // Initialize the plotter interface
    Plotter_ fb(WIDTH,HEIGHT);
    Drawer drawer(&fb);
    drawer.clear();

    // Intialize the benchmark
    Time timekeeper;

    // Intialize the light sources
    AmbientLight ambient = {{100,100,100}};
    std::vector<PointLight> light;;
    {
        PointLight b = {{-100,100,100,0},{30000,0,0}};
        PointLight g = {{0,100,100,0},{0,25000,0}};
        PointLight r = {{20,20,20,0},{0,0,30000}};
        light.push_back(b);
        light.push_back(g);
        light.push_back(r);
    }

    // Initialize the object
    Object obj(argv[1]);
    obj.material.ka = {0.1,0.1,0.1};
    obj.material.kd = {0.5,0.5,0.5};
    obj.material.ks = {0.5,0.5,0.5};
    obj.material.ns = 30;
    //obj.vmatrix() /= TfMatrix::translation({0,0,-2});

    unsigned nSurfs = obj.surfaceCount();
    unsigned nVerts = obj.vertexCount();

    std::cout << "FPS limit:\t" << FPS << "\n" << std::endl;

    while (!fb.checkTerm()) {

        // Start benchmark time
        timekeeper.start();

        // Intialize a transformation matrix to transform object
        Matrix<float> rotator = TfMatrix::rotation(Math::toRadian(2),Vector(1,1,0),Vector(0,0,0));
        // Apply transformation to object
        obj.vmatrix() /= rotator;
        // Apply transformation to vertex normals (only rotation type)
        obj.nmatrix() /= rotator;

        // Create a copy of object
        Object copy = obj;

        // camera
        // view reference point
        Vector vrp(0,0,50);
        // view plane normal
        Vector vpn= Vector(0,0,0) - vrp;
        // View up
        Vector vup(0,1,-1);

        // Apply perspective projection and camera projection on copy
        // TODO: this could only replicate the vertex as
        // other properties are constant
        copy.vmatrix() /=
            TfMatrix::perspective2(95,(float)WIDTH/HEIGHT,10000,5)
            * TfMatrix::lookAt(vrp,vpn,vup);

        // Change the homogenous co-ordinates to
        // normalized co-ordinate and  to device co-ordinate
        for (unsigned i=0; i<nVerts; i++) {
            // Perspective divide
            // NOTE: normalization is also done during the projection transformation
            copy(0,i) /= copy(3,i);
            copy(1,i) /= copy(3,i);
            copy(2,i) /= copy(3,i);

            // Converting normalized Z co-ordinate [-1,1] to depthmap [1,0]*depth
            // Visible part is between n and f.
            // The z map is shown below
            // space:        0       n       f
            // value: -ve   inf      1       0   -ve
            // NOTE: n and f have negative values
            // Normalized co-rodinate is in right hand system
            // camera is towards negative Z axis
            copy(2,i) = (-copy(2,i)*0.5 + 0.5)*ScreenPoint::maxDepth;
            // Change the normalized X Y co-ordinates to device co-ordinate
            copy(0,i) = copy(0,i)*WIDTH + WIDTH/2;
            copy(1,i) = HEIGHT - (copy(1,i)*HEIGHT + HEIGHT/2);
        }


        // Detect backfaces in normalized co-ordinates
        bool* show;
        if(BACKFACEDETECTION){
        show = new bool[nSurfs];
        memset(show,1,nSurfs*sizeof(bool));
        //if(UNBOUNDED || BACKFACEDETECTION){
            // Required for backface detection
            // When backface is off, still can be useful for unbounded objects
            // to reverse the direction
            for(int i=0;i<nSurfs;i++) {
                Vector normal = copy.getSurfaceNormal(i);
                if( normal.z >= 0)
                    show[i] = false;
            }
        }

        unsigned iteration = GOURAD?nVerts:nSurfs;
        // Stores Color for each vertices
        Color* colors = new Color[iteration];

        // Gourad-shading : calculate the colors to shade each surface with
        for(auto i=0;i<iteration;i++) {
            // Intensity
            Coeffecient intensity;
            // Ambient lighting
            intensity.r = ambient.intensity.r*obj.material.ka.r;
            intensity.g = ambient.intensity.g*obj.material.ka.g;
            intensity.b = ambient.intensity.b*obj.material.ka.b;

            Vector normal = GOURAD?obj.getVertexNormal(i):obj.getSurfaceNormal(obj.getSurface(i));
            Vector position = GOURAD?obj.getVertex(i).normalized():obj.getSurfaceCentroid(i).normalized();

            // TODO proper gourad shading ie. load normal vectors
            // before using this technique
            // TODO some bad pixels seen on the boundaries
            //if(UNBOUNDED && show[i]==false){
            //    normal = -normal;
            //}

            for(int i=0; i<light.size(); i++){

                Coeffecient decintensity= light[i].intensityAt(position);
                Vector decdirection = light[i].directionAt(position);

                // Diffused lighting
                float cosine = Vector::cosine((decdirection*(-1)),normal);
                // This is to be done so that there won't be symmetric lighting
                if(cosine < 0)
                    continue;
                intensity.r += decintensity.r*obj.material.kd.r*cosine;
                intensity.g += decintensity.g*obj.material.kd.g*cosine;
                intensity.b += decintensity.b*obj.material.kd.b*cosine;

                // Specular ligting
                Vector half = (decdirection + position)*(-1);
                float cosine2 = Vector::cosine(half,normal);
                float cosineNs = std::pow( cosine2 , obj.material.ns );
                if(cosine2 < 0)
                    continue;
                intensity.r += decintensity.r*obj.material.ks.r*cosineNs;
                intensity.g += decintensity.g*obj.material.ks.g*cosineNs;
                intensity.b += decintensity.b*obj.material.ks.b*cosineNs;
            }
            // The intensity gathered can be seen the reflected light source
            PointLight ref = {position,intensity};
            Coeffecient refintensity = ref.intensityAt(vrp);
            Uint8 clrR = Math::min( refintensity.r ,1.0f) * 255;
            Uint8 clrG = Math::min( refintensity.g ,1.0f) * 255;
            Uint8 clrB = Math::min( refintensity.b ,1.0f) * 255;
            colors[i] =  {clrR,clrG,clrB,255};
        }

        // Clear framebuffer, we're about to plot
        drawer.clear();

        // Fill the surfaces
        for (int i=0; i<nSurfs; i++) {

            if(BACKFACEDETECTION && !show[i])
                continue;

            int index = copy.getSurface(i).x;
            ScreenPoint a(copy.getVertex(index),colors[GOURAD?index:i]);

            index = copy.getSurface(i).y;
            ScreenPoint b(copy.getVertex(index),colors[GOURAD?index:i]);

            index = copy.getSurface(i).z;
            ScreenPoint c(copy.getVertex(index),colors[GOURAD?index:i]);

            if (GOURAD)
                drawer.fillD(a,b,c);
            else
                drawer.fillD(a,b,c,colors[i]);
        }

        if(BACKFACEDETECTION)
            delete []show;
        delete []colors;

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
