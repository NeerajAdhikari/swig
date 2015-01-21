#ifndef __SDLPLOT__
#define __SDLPLOT__

#include <SDL2/SDL.h>
#include <cstring>

#include "Point.h"
#include "ex.h"

// Class SDLPlotter is a plotting and windowing interface used
// by the rest of the system. It implements a uniform interface
// for plotting, using SDL. Any other class written for the same
// purpose must have exactly the same public interface.
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
    void plot(unsigned x, unsigned y, Color pt, bool composite=false);
    void plot(ScreenPoint pt, bool composite=false);
    Color getPixel(unsigned x, unsigned y);
    Uint32 RGBA(Color pt);

    bool checkTerm();
    // Update screen.
    void update();
    // Blur the framebuffer
    void blur();
    // Clear the screen with black
    void clear();

    unsigned width() const;
    unsigned height() const;
    Color black;
    Color white;
};

#endif
