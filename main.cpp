#include<iostream>
#include<cstring>

#include<SDL2/SDL.h>

#include "mathematics/Vector.h"
#include "mathematics/Matrix.h"
#include "TfMatrix.h"

#include "Drawer.h"

#include "PointLight.h"
#include "AmbientLight.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"

#include "misc/Time.h"

// Initialize constant parameters
const uint16_t WIDTH = 800;
const uint16_t HEIGHT = 600;
const uintmax_t FPS = 100;
const uintmax_t DELAY = 1e6 / FPS;

int main(int argc, char*argv[]) {

    if (argc < 2) {
        std::cout<<"Usage: "<<argv[0]<<" filename"<<std::endl;
        return 1;
    }

    // Initialize the plotter interface
    Plotter_ fb(WIDTH, HEIGHT);
    Drawer drawer(&fb);
    Shader shader(&drawer);

    // Intialize the ambient light
    AmbientLight ambient = {{100, 100, 100}};
    shader.setAmbient(ambient);

    // Intialize point light sources
    PointLight red = {{ -1000, 1000, 1000, 0}, {200000, 0, 0}};
    PointLight green = {{0, 1000, 1000, 0}, {150000,150000,150000}};
    PointLight blue = {{0, -1000, 1000, 0}, {0, 0, 200000}};
    shader.addLight(&red);
    shader.addLight(&green);
    shader.addLight(&blue);

    // Initialize the material of object
    Coeffecient ka = Coeffecient(0.1, 0.1, 0.1);
    Coeffecient kd = Coeffecient(0.5, 0.5, 0.5);
    Coeffecient ks = Coeffecient(0.5, 0.5, 0.5);
    Material m(ka, kd, ks, 140);

    Material groundMat(Coeffecient(0.1,0.2,0.05),
            Coeffecient(0.6,0.8,0.1),
            Coeffecient(0.6,0.8,0.1),20);

    // Initialize the object
    Object plane(argv[1], m, Shading::flat, true, true);
    Object cube("resources/cube.obj", m, Shading::flat);
    /*Object ground("resources/ground.obj",m,Shading::flat,
            true,false);*/
    cube.vmatrix() /= TfMatrix::translation({0, 3, 0, 0});
    plane.vmatrix() /= TfMatrix::translation({-4, 8, 0, 0});
    Object ground(4,groundMat,Shading::flat,true,false);
    ground.setVertex(0,{10,0,-10,1});
    ground.setVertex(1,{10,0,10,1});
    ground.setVertex(2,{-10,0,10,1});
    ground.setVertex(3,{-10,0,-10,1});
    ground.setSurface({0,2,1});
    ground.setSurface({0,3,2});
    shader.addObject(&cube);
    shader.addObject(&plane);
    shader.addObject(&ground);

    // Initialize camera
    // view-reference point, view-plane normal, view-up vector
    /*Vector vxrp(0,5,10);
    Vector vxpn = Vector(0,0,0)-cam.vrp;
    Vector vxup(0,1,0);*/
    Camera cam({0, 5, 5}, {0, -5, -5}, {0, 1, 0});
    shader.setCamera(cam);

    // Initialize SDL events
    SDL_Event event;
    const Uint8*keys = SDL_GetKeyboardState(NULL);

    // Intialize the benchmark for fps
    Time timekeeper(DELAY);
    while (!fb.checkTerm()) {

        // SDL EVENTS
        if (keys[SDL_GetScancodeFromKey(SDLK_w)])
            cam.vrp += cam.vpn.normalized() / 5;
        else if (keys[SDL_GetScancodeFromKey(SDLK_s)])
            cam.vrp -= cam.vpn.normalized() / 5;
        else if (keys[SDL_GetScancodeFromKey(SDLK_a)])
            cam.vrp -= (cam.vpn * cam.vup).normalized() / 5;
        else if (keys[SDL_GetScancodeFromKey(SDLK_d)])
            cam.vrp += (cam.vpn * cam.vup).normalized() / 5;

        // For cirualar camera movement due to direction
        // repositioning
        cam.vpn = -cam.vrp;

        // Start benchmark time
        timekeeper.start();

        // Make shader do the magic
        shader.setCamera(cam);
        shader.draw();
        //break;

        // wait for some time to maintain delay
        timekeeper.wait();
    }
    return 0;
}
