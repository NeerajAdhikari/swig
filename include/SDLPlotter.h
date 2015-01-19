#ifndef __SDLPLOT__
#define __SDLPLOT__

#include <SDL2/SDL.h>
#include <cstring>

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
    Color getPixel(unsigned x, unsigned y);

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

#endif
