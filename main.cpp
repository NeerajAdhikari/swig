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
const uint16_t WIDTH = 600;
const uint16_t HEIGHT = 400;
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
    PointLight red ({{0,80,32,1},{0,-80,-32,0},
        {0,0,1,0}}, {8000, 8000, 8000});
    PointLight green({{0, 1000, 1000, 0},{0,-1000,-1000,0},
        {0,1,0,0}}, {0,150000,0});
    PointLight blue({{0, -1000, 1000, 0},{0,1000,-1000,0},
        {0,1,0,0}}, {0, 0, 200000});
    shader.addLight(&red);
    shader.addLight(&green);
    shader.addLight(&blue);

    // Initialize the material of object
    Material planeMat(Coeffecient(0.1, 0.1, 0.1),
            Coeffecient(0.6, 0.1, 0.2),
            Coeffecient(0.6, 0.1, 0.2), 140);

    Material groundMat(Coeffecient(0.1,0.2,0.05),
            Coeffecient(0.6,0.8,0.1),
            Coeffecient(0.6,0.8,0.1),20);

    Material treeMat(Coeffecient(0.1,0.1,0.1),
            Coeffecient(0.2,0.4,0.0),
            Coeffecient(0.2,0.4,0.0),80);


    // Initialize the object
    Object plane(argv[1], planeMat, Shading::gouraud, true, true);
    // plane.vmatrix()/=TfMatrix::scaling({5,5,5,1},{0,0,0,1});
    Matrix<float> placePlane = TfMatrix::translation({0, 12, 0, 0});
    Matrix<float> unplacePlane = TfMatrix::translation({0, -12, 0, 0});
    plane.vmatrix() /= placePlane;

    Object ground("resources/terrain.obj",groundMat,Shading::flat,
            true,false);

    Object tree("resources/tree.obj",treeMat, Shading::gouraud,
            true,false);
    tree.vmatrix() /= TfMatrix::translation({6, 1, 2, 0});

    /*
    Object ground(4,groundMat,Shading::flat,true,false);
    ground.setVertex(0,{10,0,-10,1});
    ground.setVertex(1,{10,0,10,1});
    ground.setVertex(2,{-10,0,10,1});
    ground.setVertex(3,{-10,0,-10,1});
    ground.setSurface({0,2,1});
    ground.setSurface({0,3,2});
    */

    // Add objects to Shader
    shader.addObject(&plane);
    shader.addObject(&tree);
    shader.addObject(&ground);

    // Initialize camera
    // view-reference point, view-plane normal, view-up vector
    Camera cam({27.8404,37.8099,25.767},
            {-27.8404,-37.8099,-25.767}, {0, 1, 0});


    shader.setCamera(cam);
    Matrix<float> translator = TfMatrix::translation(
            {0.05,0,0.05,0});

    // Initialize SDL events
    const Uint8*keys = SDL_GetKeyboardState(NULL);

    // Intialize the benchmark for fps
    Time timekeeper(DELAY);
    if (red.shadow_buffer==NULL)
        red.initShadowBuffer({800,800});
    red.magic = 0.0004;
    while (!fb.checkTerm()) {
        //plane.vmatrix() /= translator;

        // SDL EVENTS
        if (keys[SDL_GetScancodeFromKey(SDLK_w)])
            cam.vrp += cam.vpn.normalized() / 5;
        else if (keys[SDL_GetScancodeFromKey(SDLK_s)])
            cam.vrp -= cam.vpn.normalized() / 5;
        else if (keys[SDL_GetScancodeFromKey(SDLK_a)]){
            cam.vrp -= (cam.vpn * cam.vup).normalized() / 5;
            std::cout << cam.vrp.x << " "<< cam.vrp.y << " "<< cam.vrp.z << "\n" << std::endl;

        } else if (keys[SDL_GetScancodeFromKey(SDLK_d)])
            cam.vrp += (cam.vpn * cam.vup).normalized() / 5;

        else if (keys[SDL_GetScancodeFromKey(SDLK_g)])
            plane.setShading(Shading::gouraud);
        else if (keys[SDL_GetScancodeFromKey(SDLK_f)])
            plane.setShading(Shading::flat);

        else if (keys[SDL_GetScancodeFromKey(SDLK_1)]){
            plane.backface(false);
            plane.bothsides(false);
            std::cout << "Bounded\n" << std::endl;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_2)]){
            plane.backface(true);
            plane.bothsides(false);
            std::cout << "Bounded Backface\n" << std::endl;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_3)]){
            plane.backface(true);
            plane.bothsides(true);
            std::cout << "Unbounded\n" << std::endl;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_n)]) {
            red.magic /= 1.5;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_m)]) {
            red.magic *= 1.5;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_i)]) {
            plane.vmatrix() /= TfMatrix::translation({0,0,0.2,0});
        } else if (keys[SDL_GetScancodeFromKey(SDLK_k)]) {
            plane.vmatrix() /= TfMatrix::translation({0,0,-0.2,0});
        } else if (keys[SDL_GetScancodeFromKey(SDLK_j)]) {
            plane.vmatrix() /= placePlane*TfMatrix::rotationz(0.1)
                *TfMatrix::rotationy(0.1)*unplacePlane;
        } else if (keys[SDL_GetScancodeFromKey(SDLK_l)]) {
            plane.vmatrix() /= placePlane*TfMatrix::rotationz(-0.1)
                *TfMatrix::rotationy(-0.1)*unplacePlane;
        }

        // For cirualar camera movement due to direction
        // repositioning
        cam.vpn = -cam.vrp;

        // Start benchmark time
        timekeeper.start();

        // Make shader do the magic
        shader.setCamera(cam);
        /*for (int i=0; i<red.dim.y; i++) {
        for (int j=0;j<red.dim.x;j++) {
            int val = (((long double)red.depthAt(i,j))/INT32_MAX)*0xff;
            fb.plot(i,j,{val,val,val,0xff},false);
        }
        }
        if (red.onShadow({10,0,10,1}))
            std::cout<<"OnShadow"<<std::endl;
        else
            std::cout<<"nOShadow"<<std::endl;*/
        red.updateShadowBuffer(&shader,&fb);
        shader.draw();

        //break;
        //fb.update();

        // wait for some time to maintain delay
        timekeeper.wait();
    }
    return 0;
}
