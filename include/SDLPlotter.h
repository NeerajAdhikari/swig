#ifndef __SDLPLOT__
#define __SDLPLOT__

#include <SDL2/SDL.h>

#include "Point.h"
#include "ex.h"

// Class SDLPlotter is a plotting and windowing interface used
// by the rest of the system. It implements a uniform interface
// for plotting, using SDL.
class SDLPlotter {

    private:
    SDL_Window* window;
    SDL_Surface* screen;
    unsigned m_width, m_height;     // Screen dimensions

    // Get the exact location to put a pixel
    Uint8* getLocation(unsigned x, unsigned y);

    public:
    SDLPlotter(unsigned w, unsigned h);
    ~SDLPlotter();
    // Plot at the given x,y position.
    void plot(unsigned x, unsigned y, Color pt);
    void plot(ScreenPoint pt);

    bool checkTerm();
    // Update screen.
    void update();
    // Blur the framebuffer
    void blur();
    // Clear the screen with black
    void clear();
    // Draw a line from start to end
    void line(const ScreenPoint& start,const ScreenPoint& end);
    // Fill the triangle bounded by pt1, pt2 and pt3
    void fill(ScreenPoint pt1, ScreenPoint pt2,
            ScreenPoint pt3, Color fillcolor);
    // Draw a horizontal line between (xs,y) and (xe,y)
    void hLine(unsigned y, unsigned xs, unsigned xe, Color cl);

    unsigned width() const;
    unsigned height() const;
    Color black;
    Color white;
};

// Construct.
SDLPlotter::SDLPlotter(unsigned w, unsigned h)
    : white({0xff,0xff,0xff,0xff}),black({0,0,0,0xff}),
    m_width(w), m_height(h)
{
    if (SDL_Init(SDL_INIT_VIDEO)<0)
        throw ex::InitFailure();

    window = SDL_CreateWindow("SWIG Test", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, m_width, m_height,
            SDL_WINDOW_SHOWN);
    if (window == NULL)
        throw ex::InitFailure();

    screen = SDL_GetWindowSurface(window);

    // Bytes Per Pixel MUST be 4
    if (screen->format->BytesPerPixel!=4)
        throw ex::InitFailure();
}

// Deconstruct
SDLPlotter::~SDLPlotter() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Plot at the given x,y position.
inline void SDLPlotter::plot(unsigned x, unsigned y, Color pt) {
    if (x<screen->w && y<screen->h)
    *(Uint32*)(screen->pixels + y*screen->pitch + x*4)=
        SDL_MapRGB(screen->format,pt.red,pt.green,pt.blue);
}

void SDLPlotter::plot(ScreenPoint pt) {
    plot(pt.x,pt.y,pt.color);
}

// Get memory location of a particular x,y position in framebuffer
Uint8* SDLPlotter::getLocation(unsigned x, unsigned y) {
    return (Uint8*)screen->pixels + y*screen->pitch + x*4;
}

// Clear scren with black
void SDLPlotter::clear() {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
}

// Update the screen
void SDLPlotter::update() {
    SDL_UpdateWindowSurface(window);
}

bool SDLPlotter::checkTerm() {
    SDL_Event e;
    if (SDL_PollEvent(&e)==0) {
        return false;
    } else if (e.type==SDL_QUIT) {
        return true;
    } else {
        return false;
    }
}

unsigned SDLPlotter::width() const {
    return m_width;
}

unsigned SDLPlotter::height() const {
    return m_height;
}

#endif
