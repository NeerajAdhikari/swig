#include <iostream>
#include <cstring>

#include <SDL2/SDL.h>

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

int main(int argc, char* argv[]) {

    if (argc<2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    Color black = {0,0,0,255};
    Color white = {255,255,255,255};
    Color badcolor = {255,0,255,255};

    // Gourad Shading
    bool GOURAD = false;
    if( argc > 2 && std::strcmp(argv[2],"gourad")==0 )
        GOURAD = true;

    // Enable two face
    bool TWOFACE = false;
    if( argc > 3 && std::strcmp(argv[3],"unbounded")==0 )
        TWOFACE = true;

    // Bacface detection
    bool BACKFACEDETECTION = false;
    if( argc > 4 && std::strcmp(argv[4],"backface")==0 )
        BACKFACEDETECTION = true;

    // Twoface or unbounded requires backfacedetection
    // for enabling overwrite which helps remove bad pixels
    if(TWOFACE)
        BACKFACEDETECTION = true;


    // Intialize the benchmark
    Time timekeeper;

    // Initialize the plotter interface
    Plotter_ fb(WIDTH,HEIGHT);
    Drawer drawer(&fb);

    // Intialize the ambient light sources
    AmbientLight ambient = {{100,100,100}};

    // Intialize the point light sources
    std::vector<PointLight> light;
    light.push_back({{-1000,1000,1000,0},  {200000,0,0}});
    light.push_back({{0,1000,1000,0},     {0,150000,0}});
    light.push_back({{0,0,1000,0},       {0,0,200000}});
    // light.push_back({{0,0,100,0},    {100000,100000,10000}});

    // Initialize the object
    Object obj(argv[1]);
    obj.material.ka = {0.1,0.1,0.1};
    obj.material.kd = {0.5,0.5,0.5};
    obj.material.ks = {0.5,0.5,0.5};
    obj.material.ns = 140;
    //obj.vmatrix() /= TfMatrix::translation({0,4,0});
    unsigned nSurfs = obj.surfaceCount();
    unsigned nVerts = obj.vertexCount();

    // Initialize camera
    // View reference point
    // View plane normal
    // View up vector
    Vector vrp(0,5,10);
    Vector vpn = Vector(0,0,0) - vrp;
    Vector vup(0,1,0);

    float n = 0;
    float avg = 0;

    SDL_Event event;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    while (true) {

        // Start benchmark time
        timekeeper.start();

        // VERTEX SHADER
        // Apply object transformation
        // Matrix<float> rotator = TfMatrix::rotation(Math::toRadian(2),Vector(1,1,0),Vector(0,0,0));
        // obj.vmatrix() /= rotator;
        // obj.nmatrix() /= rotator;

        // SDL EVENTS
        if (SDL_PollEvent(&event))
            if(event.type == SDL_QUIT) break;
        if (keys[SDL_GetScancodeFromKey(SDLK_w)]) vrp += vpn.normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_s)]) vrp -= vpn.normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_a)]) vrp -= (vpn * vup).normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_d)]) vrp += (vpn * vup).normalized()/5;
        // To rotation
        vpn = -vrp;

        // VERTEX SHADER
        // Get the vertex copy matrix
        Matrix<float>& copyalias = obj.vcmatrix();
        // Apply (perspective projection & normalization) and camera projection
        copyalias /=
            TfMatrix::perspective2(95,(float)WIDTH/HEIGHT,10000,5)
            * TfMatrix::lookAt(vrp,vpn,vup);
        // Perspective divide : homogenous co-ordinates to normalized co-ordinate
        for (unsigned i=0; i<nVerts; i++) {
            copyalias(0,i) /= copyalias(3,i);
            copyalias(1,i) /= copyalias(3,i);
            copyalias(2,i) /= copyalias(3,i);
            copyalias(3,i) = 1.0;
        }
        // Change Normalized co-ordinate system to device co-ordinate system
        copyalias /= TfMatrix::toDevice(WIDTH,HEIGHT,ScreenPoint::maxDepth);

        // SURFACE SHADER
        // Detect backfaces in normalized co-ordinates
        bool* show = new bool[nSurfs];
        if(BACKFACEDETECTION){
            memset(show,1,nSurfs*sizeof(bool));
            // Required for backface detection
            // When backface detection is diasabled, still can be useful for TWOFACE objects
            // to reverse the direction
            for(int i=0;i<nSurfs;i++) {
                // This normal is a special kind of normal, it uses x and y of
                // the projected matrix so as to get orthogonal projection system
                // but original z value for depth better depth calculation
                Vector normal = obj.getSurfaceNormalDistorted(obj.getSurface(i));
                if( normal.z >= 0)
                    show[i] = false;

                // OLD WAY
                //Vector normal = obj.getSurfaceNormal(obj.getSurface(i));
                //Vector position = obj.getSurfaceCentroid(i);
                //if( Vector::cosine((position-vrp),normal) > 0)
                //    show[i] = true;
            }
        }

        // Vertex / Surface Shader
        // Gourad or Flat shading
        unsigned iteration = GOURAD?nVerts:nSurfs;
        // Stores Color for each vertices
        Color* colors = new Color[iteration];

        // Calcualte Shading
        for(auto i=0;i<iteration;i++) {

            Vector normal = GOURAD?
                obj.getVertexNormal(i):
                obj.getSurfaceNormal(obj.getSurface(i));

            Vector position = GOURAD?
                obj.getVertex(i):
                obj.getSurfaceCentroid(i);


            // In gourad, we can't use show[i] because it signifies surface
            // and not vertices, there is no thing as hidden vertex
            // A vertex may be discarded if it isn't used in any visible
            // surface which is harder to determine so using such condition
            if(GOURAD){
                // Inverting the back surfaces for unbounded objects
                if( TWOFACE && Vector::cosine((position-vrp),normal) > 0)
                    normal = normal * -1;
            } else {
                if( TWOFACE && !show[i])
                    normal = normal * -1;
                if ( BACKFACEDETECTION && !TWOFACE && !show[i])
                    continue;
            }

            // Ambient lighting
            Coeffecient intensity = ambient.intensity * obj.material.ka;
            for(int i=0; i<light.size(); i++){
                Coeffecient decintensity= light[i].intensityAt(position);
                Vector decdirection = light[i].directionAt(position);
                // Diffused lighting
                {
                    float cosine = Vector::cosine((decdirection*(-1)),normal);
                    // Check to remove symmetric lighting
                    if(cosine <= 0)
                        continue;
                    intensity += (decintensity * obj.material.kd) * cosine;
                }
                // Specular ligting
                {
                    Vector half = (decdirection.normalized() + (position-vrp).normalized())*(-1);
                    float cosine = Vector::cosine(half,normal);
                    // Check to remove symmetric lighting
                    if(cosine <= 0)
                        continue;
                    intensity += (decintensity * obj.material.ks) * std::pow(cosine, obj.material.ns);
                }
            }
            // The reflection surface can be seen as a light source to camera
            colors[i] = PointLight({position,intensity}).intensityAt(vrp);
        }

        // Clear framebuffer, we're about to plot
        drawer.clear(badcolor);

        // Fill the surfaces
        for (int i=0; i<nSurfs; i++) {

            if( !TWOFACE && BACKFACEDETECTION && !show[i])
                continue;

            int index = obj.getSurface(i).x;
            ScreenPoint a(obj.getCopyVertex(index),colors[GOURAD?index:i]);

            index = obj.getSurface(i).y;
            ScreenPoint b(obj.getCopyVertex(index),colors[GOURAD?index:i]);

            index = obj.getSurface(i).z;
            ScreenPoint c(obj.getCopyVertex(index),colors[GOURAD?index:i]);

            // show[i] signifies backface detection
            // for unbounded objects, overwrite must be diable for
            // backface surfaces
            drawer.fillD(a,b,c,GOURAD,show[i]);
        }

        // Update framebuffer
        drawer.update();

        // delete temporaries
        delete []show;
        delete []colors;

        // Stop benchmark time
        uintmax_t ti = timekeeper.time();
        float real_fps = 1e6 / ti;
        n++;
        avg = (avg*(n-1)+real_fps)/n;
        std::cout << DELETE << FPS << " : " << avg << " fps"<< std::endl;


        if( ti < DELAY){
            // Convert us to ms and delay
            SDL_Delay((DELAY-ti)/1000);
        }
    }
    return 0;
}
