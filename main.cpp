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

    // Initialize the plotter interface
    Plotter_ fb(WIDTH,HEIGHT);
    Drawer drawer(&fb);

    // Initialize some good colors
    Color black = {0,0,0,255};
    Color white = {255,255,255,255};
    Color badcolor = {255,0,255,255};

    // Intialize the ambient light
    AmbientLight ambient = {{100,100,100}};

    // Intialize point light sources
    std::vector<PointLight> light;
    light.push_back({{-1000,1000,1000,0},  {200000,0,0}});
    light.push_back({{0,1000,1000,0},     {0,150000,0}});
    light.push_back({{0,0,1000,0},       {0,0,200000}});

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
    // view-reference point, view-plane normal, view-up vector
    Vector vrp(0,5,10);
    Vector vpn = Vector(0,0,0) - vrp;
    Vector vup(0,1,0);

    // Initialize SDL events
    SDL_Event event;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    // Intialize the benchmark for fps
    Time timekeeper;
    float n = 0, avg = 0;

    while (true) {

        // SDL EVENTS
        if (SDL_PollEvent(&event))
            if(event.type == SDL_QUIT) break;
        if (keys[SDL_GetScancodeFromKey(SDLK_w)])
            vrp += vpn.normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_s)])
            vrp -= vpn.normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_a)])
            vrp -= (vpn * vup).normalized()/5;
        if (keys[SDL_GetScancodeFromKey(SDLK_d)])
            vrp += (vpn * vup).normalized()/5;
        // For cirualar camera movement due to direction repositioning
        vpn = -vrp;

        // Start benchmark time
        timekeeper.start();

        // VERTEX SHADER
        // Apply object transformation
        // Matrix<float> rotator = TfMatrix::rotation(Math::toRadian(2),Vector(1,1,0),Vector(0,0,0));
        // obj.vmatrix() /= rotator;
        // obj.nmatrix() /= rotator;

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
        bool* surfaceVisibililty;
        if(BACKFACEDETECTION){
            surfaceVisibililty= new bool[nSurfs];
            memset(surfaceVisibililty,1,nSurfs*sizeof(bool));

            for(int i=0;i<nSurfs;i++) {
                // This normal is a special kind of normal, it uses x and y of
                // the projected matrix so as to get orthogonal projection system
                // but original z value for better depth calculation
                Vector normal = obj.getSurfaceNormalDistorted(obj.getSurface(i));
                if( normal.z >= 0)
                    surfaceVisibililty[i] = false;
            }
        }

        // Vertex / Surface Shader
        // Gourad or Flat shading
        unsigned iteration = GOURAD?nVerts:nSurfs;
        // Stores Color for each vertices
        Color* surfaceColor = new Color[iteration];

        // Calcualte Shading
        for(auto i=0;i<iteration;i++) {

            // An object may have surfaces of different materials
            Material& material = obj.material;

            // Normal for lighting calculation
            Vector normal = GOURAD?
                obj.getVertexNormal(i):
                obj.getSurfaceNormal(obj.getSurface(i));

            // Position for lighting calculation
            Vector position = GOURAD?
                obj.getVertex(i):
                obj.getSurfaceCentroid(i);

            // NOTE
            // surfaceVisibililty[i] is false if a surface is not visible,
            // this cannot be used for vertices visibility

            // Inverting the back surfaces for unbounded objects
            if( TWOFACE && GOURAD && Vector::cosine((position-vrp),normal) > 0)
                normal = normal * -1;
            if( TWOFACE && !GOURAD && !surfaceVisibililty[i])
                normal = normal * -1;
            // If backfacedetection then continue if suitable
            if ( !TWOFACE && !GOURAD && BACKFACEDETECTION && !surfaceVisibililty[i])
                continue;

            // Ambient lighting
            Coeffecient intensity = ambient.intensity * obj.material.ka;

            // Diffused and Specular lighting
            for(int i=0; i<light.size(); i++)
                intensity += light[i].lightingAt(position, normal, material, vrp);

            // Automatic conversion from Coeffecient to Color
            // The reflection surface can be seen as a light source to camera
            surfaceColor[i] = PointLight({position,intensity}).intensityAt(vrp);
        }

        // Clear framebuffer, we're about to plot
        drawer.clear(badcolor);

        // Fill the surfaces
        for (int i=0; i<nSurfs; i++) {

            if( !TWOFACE && BACKFACEDETECTION && !surfaceVisibililty[i])
                continue;

            int index = obj.getSurface(i).x;
            ScreenPoint a(obj.getCopyVertex(index),surfaceColor[GOURAD?index:i]);

            index = obj.getSurface(i).y;
            ScreenPoint b(obj.getCopyVertex(index),surfaceColor[GOURAD?index:i]);

            index = obj.getSurface(i).z;
            ScreenPoint c(obj.getCopyVertex(index),surfaceColor[GOURAD?index:i]);

            // overwrite is enabled for non backface surfaces
            drawer.fillD(a,b,c,GOURAD,surfaceVisibililty[i]);
        }

        // Update framebuffer
        drawer.update();

        // delete temporaries
        if(BACKFACEDETECTION)
            delete []surfaceVisibililty;
        delete []surfaceColor;

        // Stop benchmark time and calculate time
        n++;
        uintmax_t ti = timekeeper.time();
        float real_fps = 1e6 / ti;
        avg = (avg*(n-1)+real_fps)/n;
        // Display fps
        std::cout << DELETE << FPS << " : " << avg << " fps"<< std::endl;
        // Limit the fps
        if( ti < DELAY)
            SDL_Delay((DELAY-ti)/1000);
    }
    return 0;
}
