#ifndef __SDLPLOT__
#define __SDLPLOT__

#include <SDL2/SDL.h>
#include <cstring>

#include "common/ex.h"
#include "common/helper.h"
#include "ScreenPoint.h"

// Class SDLPlotter is a plotting and windowing interface used
// by the rest of the system. It implements a uniform interface
// for plotting, using SDL. Any other class written for the same
// purpose must have exactly the same public interface.
class SDLPlotter {

    private:
    SDL_Window* window;
    SDL_Surface* screen;
    unsigned m_width, m_height;     // Screen dimensions

    // Get memory location of a particular x,y position in framebuffer
    inline Uint8* getLocation(unsigned x, unsigned y) {
        return (Uint8*)screen->pixels + y*screen->pitch + x*4;
    }

    public:
    SDLPlotter(unsigned w, unsigned h);
    ~SDLPlotter();

    // Plot at the given x,y position.
    // This is the most speed-critical part of the code, as it is called
    // so many times for each frame
    inline void plot(unsigned x, unsigned y, Color pt, bool
            composite=false) {
        // Return if values out of range
        if (!(x<screen->w && y<screen->h))
            return;
        // If alpha compositing is to be done, calculate
        // new color value.
        if (composite) {
            Color prev = getPixel(x,y); prev.alpha = 0xff-pt.alpha;
            pt.red = (prev.red*prev.alpha+pt.red*pt.alpha)/0xff;
            pt.green = (prev.green*prev.alpha+pt.green*pt.alpha)/0xff;
            pt.blue = (prev.blue*prev.alpha+pt.blue*pt.alpha)/0xff;
            pt.alpha = 0xff;
        }
        // Write pixel to memory
        *((Uint32 *)screen->pixels + y*screen->pitch/4 + x) =
            RGBA(pt);
    }

    // Plot a ScreenPoint
    inline void plot(ScreenPoint pt, bool composite) {
        plot(pt.x,pt.y,pt.color,composite);
    }

    // get the Pixel value at the specified x,y position
    // TODO storage format may be machine-dependent
    inline Color getPixel(unsigned x, unsigned y) {
        Uint32 val = *((Uint32*)screen->pixels+y*screen->pitch/4+x);
        Color c;
        c.red = (Uint8)(val&0xff);
        c.green = (Uint8)((val>>8)&0xff);
        c.blue = (Uint8)((val>>16)&0xff);
        c.alpha = (Uint8)((val>>24)&0xff);
        return c;
    }

    // Return a 32-bit memory representation of the Color struct.
    // TODO storage format may be machine-dependent
    inline Uint32 RGBA(Color pt) {
        return (pt.alpha<<24)|(pt.blue<<16)|(pt.green<<8)|(pt.red);
    }
    // Update the screen
    inline void update() {
        SDL_UpdateWindowSurface(window);
    }

    // Clear scren with black
    inline void clear() {
        SDL_FillRect(screen, NULL, RGBA({255,0,255,0}));
    }

    // return screen width
    inline unsigned width() const {
        return m_width;
    }

    // return screen height
    inline unsigned height() const {
        return m_height;
    }


    bool checkTerm();

    // Update screen.
    // Blur the framebuffer
    void blur();

    Color black;
    Color white;
};

#endif
