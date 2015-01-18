#ifndef __FBPLOT__
#define __FBPLOT__

#include <SDL/SDL.h>
#include <cstring>
#include <vector>
#include <iostream>

#include "Point.h"

class SDLPlotter {
    private:
        SDL_Surface *screen;
        int bpp;
        unsigned xmax,xmin,ymax,ymin;
        Uint8* getLocation(unsigned x, unsigned y);

    public:
        SDLPlotter();
        ~SDLPlotter();
        void plot(unsigned x, unsigned y, Color pt,bool update);
        void plotlong(unsigned x, unsigned y, Color pt,
                bool update=false);
        void plot(ScreenPoint pt, bool update=true);
        void update();
        void clear();
        void line(const ScreenPoint& start,const ScreenPoint& end);
        std::vector<unsigned> linepoints(
                const ScreenPoint& start, const ScreenPoint& end);
        void fill(ScreenPoint pt1, ScreenPoint pt2,
                ScreenPoint pt3, Color fillcolor);
        void hLine(unsigned y, unsigned xs, unsigned xe, Color cl);

        Color black;
        Color white;
};

SDLPlotter::SDLPlotter():white({255,255,255,255}),black({0,0,0,255}),
    xmax(0), ymax(0) {
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(800,600,8,SDL_SWSURFACE|SDL_ANYFORMAT);
    bpp = screen->format->BytesPerPixel;
}

SDLPlotter::~SDLPlotter() {
    SDL_Quit();
}

inline void SDLPlotter::plot(unsigned x, unsigned y, Color pt, bool update=false) {
    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
    if (x<screen->w && y<screen->h)
        *(Uint32*)(screen->pixels + y*screen->pitch + x*bpp) =
            SDL_MapRGB(screen->format,pt.red,pt.green,pt.blue);
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
}


void SDLPlotter::plotlong(unsigned x, unsigned y, Color pt, bool update) {
    if (x>screen->w || y>screen->h)
        return;
    if (x>xmax) xmax = x;
    if (x<xmin) xmin = x;
    if (y>ymax) ymax = y;
    if (y<ymin) ymin = y;
    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
    Uint8* p = getLocation(x,y);
    Uint32 pixel=SDL_MapRGB(screen->format,pt.red,pt.green,pt.blue);
    switch (bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16*)p = pixel;
            break;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32*)p  = pixel;
            break;
    }
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
    if (update)
        //SDL_UpdateRect(screen,(xmin+xmax)/2,(ymin+ymax)/2,xmax-xmin,
        //        ymax-ymin);
        SDL_UpdateRect(screen,0,0,0,0);
}

Uint8* SDLPlotter::getLocation(unsigned x, unsigned y) {
    return (Uint8*)screen->pixels + y*screen->pitch + x*bpp;
}

void SDLPlotter::plot(ScreenPoint pt, bool update) {
    plotlong(pt.x,pt.y,pt.color,update);
}

void SDLPlotter::clear() {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,
                0,0,0));
}

void SDLPlotter::update() {
    //SDL_UpdateRect(screen,(xmin+xmax)/2,(ymin+ymax)/2,xmax-xmin,
    //        ymax-ymin);
    SDL_UpdateRect(screen,0,0,0,0);
}

void SDLPlotter::line(const ScreenPoint& start,
        const ScreenPoint& end) {
    int dx, dy, x, y, xc, yc;
    int D,twodydx,twodxdy,twody,twodx;

    dx = abs(end.x-start.x); dy = abs(end.y-start.y);
    if (dx==0) xc=0;
    else xc = (start.x>end.x)?-1:1;
    if (dy==0) yc=0;
    else yc = (start.y>end.y)?-1:1;

    x = start.x;
    y = start.y;
    plot(x,y,start.color,false);

    if (dx>dy) {
        D = 2*dy-dx;
        twodydx = 2*(dy-dx);
        twody = 2*dy;

        while (x!=end.x) {
            x+=xc;
            if (D>0) {
                y+=yc;
                plot(x,y,start.color,false);
                D += twodydx;
            } else {
                plot(x,y,start.color,false);
                D += twody;
            }
        }

    } else {
        D = 2*dx-dy;
        twodxdy = 2*(dx-dy);
        twodx = 2*dx;

        while (y!=end.y) {
            y+=yc;
            if (D>0) {
                x+=xc;
                plot(x,y,start.color,false);
                D += twodxdy;
            } else {
                plot(x,y,start.color,false);
                D += twodx;
            }
        }
    }
}

std::vector<unsigned> SDLPlotter::linepoints(const ScreenPoint& start,
        const ScreenPoint& end) {
    int dx, dy, x, y, xc, yc;
    int D,twodydx,twodxdy,twody,twodx;

    dx = abs(end.x-start.x); dy = abs(end.y-start.y);
    if (dx==0) xc=0;
    else xc = (start.x>end.x)?-1:1;
    if (dy==0) yc=0;
    else yc = (start.y>end.y)?-1:1;
    std::vector<unsigned> pts(dy+1);

    x = start.x;
    y = start.y;
    pts[abs(y-start.y)] = x;

    if (dx>dy) {
        D = 2*dy-dx;
        twodydx = 2*(dy-dx);
        twody = 2*dy;

        while (x!=end.x) {
            x+=xc;
            if (D>0) {
                y+=yc;
                pts[abs(y-start.y)] = x;
                D += twodydx;
            } else {
                pts[abs(y-start.y)] = x;
                D += twody;
            }
        }

    } else {
        D = 2*dx-dy;
        twodxdy = 2*(dx-dy);
        twodx = 2*dx;

        while (y!=end.y) {
            y+=yc;
            if (D>0) {
                x+=xc;
                pts[abs(y-start.y)] = x;
                D += twodxdy;
            } else {
                pts[abs(y-start.y)] = x;
                D += twodx;
            }
        }
    }
    return pts;
}


void SDLPlotter::fill(ScreenPoint pt1, ScreenPoint pt2, ScreenPoint pt3, Color fillcolor) {
    ScreenPoint start, mid, end;
    if (pt1.y<=pt2.y && pt1.y<=pt3.y) {
        start = pt1;
        if (pt2.y<=pt3.y) {
            mid = pt2;
            end = pt3;
        } else {
            mid = pt3;
            end = pt2;
        }
    }
    else if (pt2.y<=pt1.y && pt2.y<=pt3.y) {
        start = pt2;
        if (pt1.y<=pt3.y) {
            mid = pt1;
            end = pt3;
        } else {
            mid = pt3;
            end = pt1;
        }
    }
    else {
        start = pt3;
        if (pt1.y<=pt2.y) {
            mid = pt1;
            end = pt2;
        } else {
            mid = pt2;
            end = pt1;
        }
    }

    if (start.y==end.y) {
         hLine(start.y,start.x,mid.x,fillcolor);
         hLine(start.y,mid.x,end.x,fillcolor);
         return;
    }
    float xda,xdb,r1,r2,r3;
    r2=((float)end.x-start.x)/(end.y-(float)start.y);
    if (start.y!=mid.y) {
        r1=((float)mid.x-start.x)/((float)mid.y-start.y);
        for (auto i=start.y; i<=mid.y; i++) {
            xda = r1*(i-start.y);
            xdb = r2*(i-start.y);
            hLine(i, start.x+xda, start.x+xdb, fillcolor);
        }
    }
    if (mid.y!=end.y) {
        r3=((float)end.x-mid.x)/(end.y-(float)mid.y);
        for (auto i=mid.y; i<=end.y; i++) {
            xda = r3*(i-mid.y);
            xdb = r2*(i-start.y);
            hLine(i, mid.x+xda, start.x+xdb, fillcolor);
        }
    }
}

void SDLPlotter::hLine(unsigned y, unsigned xs, unsigned xe, Color cl) {
    if (xs>xe) {
        auto t = xs;
        xs = xe;
        xe = t;
    }
    for (auto i=0; i<=(xe-xs); i++) {
        plot(xs+i,y,cl);
    }
}

#endif
