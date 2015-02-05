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
const uint16_t width = 800;
const uint16_t height = 600;
// depth in respect to Screenpoint,
// also used inside drawer
const uint32_t depth = INT32_MAX;

const uintmax_t FPS = 100;
const uintmax_t DELAY = 1e6/FPS;

int main(int argc, char* argv[]) {

    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    // Initialize the plotter interface
    Plotter_ fb(width,height);
    Drawer drawer(&fb);
    drawer.clear();

    // Intialize the benchmark
    Time timekeeper;

    // Intialize the light sources
    // TODO point light sources are infinitely far away
    AmbientLight ambient = {{10,10,10}};
    std::vector<PointLight> light;;
    {
        PointLight b = {{-100,100,100,0},{10,0,0}};
        PointLight g = {{0,100,100,0},{0,15,0}};
        PointLight r = {{20,20,20,0},{0,0,10}};
        light.push_back(b);
        light.push_back(g);
        light.push_back(r);
    }

    // Initialize the object
    Object obj(argv[1]);
    // TODO load vertex normals for the file
    // Initialize the normal to the surfaces
    obj.initNormal();
    //obj.vmatrix() /= TfMatrix::translation({0,0,-2});
    obj.material.ka = {0.1,0.1,0.1};
    obj.material.kd = {79.5,79.5,79.5};
    obj.material.ks = {99.5,99.5,99.5};
    obj.material.ns = 30;

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

        // view reference point
        Vector vrp(0,0,10);
        // view plane normal
        Vector vpn= Vector(0,0,0) - vrp;
        // View up
        Vector vup(0,1,-1);

        // Applying projection matrix
        //proj /= TfMatrix::perspective2(95,(float)width/height,-10000,-5);
        Matrix<float> proj =
            TfMatrix::perspective2(95,(float)width/height,10000,5)
            * TfMatrix::lookAt(vrp,vpn,vup)
            * TfMatrix::translation(-vrp);

        // Create a copy of object
        Object copy = obj;
        // Apply perspective projection
        copy.vmatrix() /= proj;

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
            // NOTE: n and f have negative values as in right hand system
            // camera is towards negative Z axis
            copy(2,i) = (-copy(2,i)*0.5 + 0.5)*depth;
            // Change the normalized X Y co-ordinates to device co-ordinate
            copy(0,i) = copy(0,i)*width + width/2;
            copy(1,i) = height - (copy(1,i)*height + height/2);
        }

        // Stores Color for each vertices
        Color colors[nVerts];

        // Gourad-shading : calculate the colors to shade each surface with
        for(auto i=0;i<nVerts;i++) {
            // Intensity
            Coeffecient intensity;
            // Ambient lighting
            intensity.r = ambient.intensity.r*obj.material.ka.r;
            intensity.g = ambient.intensity.g*obj.material.ka.g;
            intensity.b = ambient.intensity.b*obj.material.ka.b;

            Vector normal = obj.getVertexNormal(i);
            Vector position = obj.getVertex(i).normalized();
            for(int i=0; i<light.size(); i++){

                Coeffecient decintensity= light[i].intensityAt(position);
                Vector decdirection = light[i].directionAt(position);

                // Diffused lighting
                float cosine = Vector::cosine((decdirection*(-1)),normal);
                // This is to be done so that there won't be symmetric lighting
                if(cosine <= 0)
                    continue;
                intensity.r += decintensity.r*obj.material.kd.r*cosine;
                intensity.g += decintensity.g*obj.material.kd.g*cosine;
                intensity.b += decintensity.b*obj.material.kd.b*cosine;

                // Specular ligting
                Vector half = (decdirection + position)*(-1);
                float cosine2 = Vector::cosine(half,normal);
                float cosineNs = std::pow( cosine2 , obj.material.ns );
                if(cosine2 <= 0)
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
