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
    bool UNBOUNDED = false;
    if( argc > 3 && std::strcmp(argv[3],"unbounded")==0 )
        UNBOUNDED = true;

    // Bacface detection
    bool BACKFACEDETECTION = false;
    if( argc > 4 && std::strcmp(argv[4],"backface")==0 )
        BACKFACEDETECTION = true;

    // Twoface or unbounded requires backfacedetection
    // for enabling overwrite which helps remove bad pixels
    if(UNBOUNDED)
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

    // Initialize the material of object
    Coeffecient ka = Coeffecient(0.1,0.1,0.1);
    Coeffecient kd = Coeffecient(0.5,0.5,0.5);
    Coeffecient ks = Coeffecient(0.5,0.5,0.5);
    Material m(ka,kd,ks,140);

    // Initialize the object
    Object plane(argv[1], m );
    Object gourd("resources/cube.obj",m);
    gourd.vmatrix() /= TfMatrix::translation({0,-5,0,0});

    std::vector <Object*> obj;
    obj.push_back( &plane );
    obj.push_back( &gourd );

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
        //Matrix<float> rotator = TfMatrix::rotation(Math::toRadian(2),Vector(0,1,0),Vector(0,0,0));

        // Apply camera projection and perspective projection transformation
        // Change homogeneous co-ordinate system to device co-ordinate system
        Matrix<float> transformation =
            TfMatrix::toDevice(WIDTH,HEIGHT,ScreenPoint::maxDepth)
            * TfMatrix::perspective(95,(float)WIDTH/HEIGHT,10000,5)
            * TfMatrix::lookAt(vrp,vpn,vup);

        for(int k=0;k< obj.size(); k++){
            //obj[k]->vmatrix() /= rotator;
            //obj[k]->vnmatrix() /= rotator;

            // Get the vertex copy matrix
            // NOTE: getting a vertex matrix will reset the
            // previous content of the copy vertex matrix
            Matrix<float>& copyalias = obj[k]->vcmatrix();
            copyalias /= transformation;

            // Perspective divide, homogenous co-ordinates to normalized co-ordinate
            // NOTE: this can be done later in life
            for (unsigned i=0; i<obj[k]->vertexCount(); i++) {
                copyalias(0,i) /= copyalias(3,i);
                copyalias(1,i) /= copyalias(3,i);
                copyalias(2,i) /= copyalias(3,i);
                copyalias(3,i) = 1.0;
            }

            // SURFACE SHADER

            // Detect backfaces in normalized co-ordinates
            if(BACKFACEDETECTION) {
                for(int i=0;i<obj[k]->surfaceCount();i++) {
                    // This normal is a special kind of normal, it uses x and y of
                    // the projected matrix so as to get orthogonal projection system
                    // but original Z value for better depth calculation
                    Vector normal = obj[k]->getDistortedSurfaceNormal(i);
                    obj[k]->getSurface(i).visible = (normal.z < 0);
                }
            }

            if(GOURAD){
                // VERTEX shader
                obj[k]->initColors(obj[k]->vertexCount());
                for(auto i=0;i<obj[k]->vertexCount();i++) {

                    // An object may have surfaces of different materials
                    const Material& material = obj[k]->material();
                    // Normal for lighting calculation
                    Vector normal = obj[k]->getVertexNormal(i);
                    // Position for lighting calculation
                    Vector position = obj[k]->getVertex(i);

                    // Inverting the back surfaces for unbounded objects
                    if( UNBOUNDED && Vector::cosine((position-vrp),normal) > 0)
                        normal *= -1;

                    // Ambient lighting
                    Coeffecient intensity = ambient.intensity * material.ka;

                    // Diffused and Specular lighting
                    for(int i=0; i<light.size(); i++)
                        intensity += light[i].lightingAt(position, normal, material, vrp);

                    // Automatic conversion from Coeffecient to Color
                    // The reflection surface can be seen as a light source to camera
                    obj[k]->getColor(i) = PointLight({position,intensity}).intensityAt(vrp);
                }
            } else {
                // SURFACE shader
                obj[k]->initColors(obj[k]->surfaceCount());
                //colors =  new Color[obj.surfaceCount()];
                for(auto i=0;i<obj[k]->surfaceCount();i++) {

                    // An object may have surfaces of different materials
                    const Material& material = obj[k]->material();
                    // Normal for lighting calculation
                    Vector normal =  obj[k]->getSurfaceNormal(i);
                    // Position for lighting calculation
                    Vector position =  obj[k]->getSurfaceCentroid(i);

                    // If backfacedetection then continue if suitable
                    if ( !UNBOUNDED && BACKFACEDETECTION && !obj[k]->getSurface(i).visible)
                        continue;
                    // Inverting the back surfaces for unbounded objects
                    else if( UNBOUNDED && !obj[k]->getSurface(i).visible)
                        normal *=  -1;

                    // Ambient lighting
                    Coeffecient intensity = ambient.intensity * material.ka;

                    // Diffused and Specular lighting
                    for(int i=0; i<light.size(); i++)
                        intensity += light[i].lightingAt(position, normal, material, vrp);

                    // Automatic conversion from Coeffecient to Color
                    // The reflection surface can be seen as a light source to camera
                    obj[k]->getColor(i) = PointLight({position,intensity}).intensityAt(vrp);

                }
            }
        }

        // Clear framebuffer, we're about to plot
        drawer.clear(badcolor);
        // Fill the surfaces
        for(int k=0;k< obj.size(); k++){
            for (int i=0; i<obj[k]->surfaceCount(); i++) {

                if( !UNBOUNDED && BACKFACEDETECTION && !obj[k]->getSurface(i).visible)
                    continue;

                int index = obj[k]->getSurface(i).x;
                ScreenPoint a(obj[k]->getCopyVertex(index),obj[k]->getColor(GOURAD?index:i));

                index = obj[k]->getSurface(i).y;
                ScreenPoint b(obj[k]->getCopyVertex(index),obj[k]->getColor(GOURAD?index:i));

                index = obj[k]->getSurface(i).z;
                ScreenPoint c(obj[k]->getCopyVertex(index),obj[k]->getColor(GOURAD?index:i));

                // overwrite is enabled for non backface surfaces
                drawer.fillD(a,b,c,GOURAD, obj[k]->getSurface(i).visible);
            }
        }
        // Update framebuffer
        drawer.update();

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
