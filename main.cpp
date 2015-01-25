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
    Time timekeeper;

    // Intialize the light sources
    AmbientLight ambient = {{10,10,10}};
    std::vector<PointLight> light;;
    {
        PointLight t = {{-1,-1,-1,0},{10,0,0}};
        PointLight m = {{1,-1,-1,0},{0,0,10}};
        PointLight n = {{0,0,1,0},{10,10,0}};
        light.push_back(m);
        light.push_back(n);
        light.push_back(t);
    }

    // Initialize the object
    Object obj(argv[1]);
    // Initialize the normal to the surfaces
    obj.initNormal();
    // material has a default constructor
    //obj.material = {0.01,0.05,0.05,270};
    unsigned nSurfs = obj.surfaceCount();
    unsigned nVerts = obj.vertexCount();

    // Initialize camera position and direction
    Vector camera_pos(0,0,15);
    Vector camera_dir(0,0,-1);
    camera_dir.normalize();

    std::cout << "FPS limit:\t" << FPS << "\n" << std::endl;
    while (!fb.checkTerm()) {

        // Start benchmark time
        timekeeper.start();

        // The transformation matrices must be dynamic and must change with the
        // use input so keeping it inside the main loop
        // The camera position is shifted to origin and rotated to align with negative z axis
        Matrix<float> proj = TfMatrix::translation(camera_pos*-1);
        if( camera_dir.z > 0)
            proj /= TfMatrix::rotationy( Math::pi - std::atan(camera_dir.x / camera_dir.magnitude()));
        else
            proj /= TfMatrix::rotationy( std::atan(camera_dir.x / camera_dir.magnitude()));
        proj /= TfMatrix::rotationx( - std::atan(camera_dir.y / std::sqrt( std::pow(camera_dir.x,2) + std::pow(camera_dir.z,2)  ) ));
        // Applying projection matrix
        proj /= TfMatrix::perspective2(95,(float)width/height,10000,5);

        // Intialize a transformation matrix to transform object
        Matrix<float> rotator = TfMatrix::rotation(Math::toRadian(2),Vector(1,1,0),Vector(0,0,0));
        // For flat shading, the colors we need to fill surfaces with

        // Apply transformation to object
        obj.vmatrix() /= rotator;
        // Apply transformation to vertex normals (only rotation type)
        obj.nmatrix() /= TfMatrix::rotation(Math::toRadian(2),{1,1,0},{0,0,0});

        // Create a copy of object
        Object copy = obj;
        // Apply perspective projection
        copy.vmatrix() /= proj;

        // Change the homogenous co-ordinates to
        // normalized co-ordinate and  to device co-ordinate
        for (unsigned i=0; i<nVerts; i++) {
            // Perspective divide
            copy(0,i) /= copy(3,i);
            copy(1,i) /= copy(3,i);
            copy(2,i) /= copy(3,i);

            // Change the normalized X Y co-ordinates to device co-ordinate
            copy(0,i) = copy(0,i)*width + width/2;
            copy(1,i) = height - (copy(1,i)*height + height/2);

            // Converting normalized Z co-ordinate [-1,1] to depthmap [1,0]*depth
            // Visible part is between n and f.
            // The z map is shown below
            //        0       n       f
            // -ve   inf      1       0   -ve
            copy(2,i) = (-copy(2,i)*0.5 + 0.5)*0xffff;
        }

        // Stores Color for each vertices
        Color colors[nVerts];
        // Gourad-shading : calculate the colors to shade each surface with
        for(auto i=0;i<nVerts;i++) {

            Vector normal = obj.getVertexNormal(i);
            Vector position = obj.getVertex(i).normalized();

            Coeffecient intensity;

            // Ambient lighting
            intensity.r = ambient.intensity.r*obj.material.ka.r;
            intensity.g = ambient.intensity.g*obj.material.ka.g;
            intensity.b = ambient.intensity.b*obj.material.ka.b;

            for(int i=0; i<light.size(); i++){

                // Diffused lighting
                float cosine = Vector::cosine((light[i].direction*(-1)),normal);
                // This is to be done so that there won't be symmetric lighting
                if(cosine <= 0)
                    continue;
                intensity.r += light[i].intensity.r*obj.material.kd.r*cosine;
                intensity.g += light[i].intensity.g*obj.material.kd.g*cosine;
                intensity.b += light[i].intensity.b*obj.material.kd.b*cosine;

                // Specular ligting
                Vector half = (light[i].direction + position)*(-1);
                float cosine2 = Vector::cosine(half,normal);
                float cosineNs = std::pow( cosine2 , obj.material.ns );
                if(cosine2 <= 0)
                    continue;
                intensity.r += light[i].intensity.r*obj.material.ks.r*cosineNs;
                intensity.g += light[i].intensity.g*obj.material.ks.g*cosineNs;
                intensity.b += light[i].intensity.b*obj.material.ks.b*cosineNs;
            }

            Uint8 clrR = Math::min(intensity.r,1.0f) * 255;
            Uint8 clrG = Math::min(intensity.g,1.0f) * 255;
            Uint8 clrB = Math::min(intensity.b,1.0f) * 255;
            colors[i] =  {clrR,clrG,clrB,255};
        }

        // Detect backfaces in normalized co-ordinates
        bool show[nSurfs];
        memset(show,0,nSurfs*sizeof(bool));
        for(int i=0;i<nSurfs;i++) {
            Vector normal = copy.getSurfaceNormal(i);
            if( normal.z < 0)
                show[i] = true;
        }

        // Clear framebuffer, we're about to plot
        drawer.clear();

        // Fill the surfaces
        for (int i=0; i<nSurfs; i++) {

            if(!show[i])
                continue;

            unsigned index;
            Vector vec;

            index = copy.getSurface(i).x;
            vec = copy.getVertex(index);
            ScreenPoint a;
            a.x = Math::round(vec.x);
            a.y = Math::round(vec.y);
            a.d = Math::round(vec.z);
            a.color =  colors[index];

            index = copy.getSurface(i).y;
            vec = copy.getVertex(index);
            ScreenPoint b;
            b.x = Math::round(vec.x);
            b.y = Math::round(vec.y);
            b.d = Math::round(vec.z);
            b.color =  colors[index];

            index = copy.getSurface(i).z;
            vec = copy.getVertex(index);
            ScreenPoint c;
            c.x = Math::round(vec.x);
            c.y = Math::round(vec.y);
            c.d = Math::round(vec.z);
            c.color =  colors[index];

            drawer.fillD(a,b,c);
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
