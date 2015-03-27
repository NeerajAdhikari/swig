#include "SDLPlotter.h"

// Construct
SDLPlotter::SDLPlotter(unsigned w, unsigned h)
    : m_width(w), m_height(h)
{
    // Initialize the video subsystem
    if (SDL_Init(SDL_INIT_VIDEO)<0)
        throw ex::InitFailure();

    // Create a window
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


// Check if a TERM signal has been sent, i.e. for example
// Alt+F4 has been pressed
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

/*
void SDLPlotter::blur() {
    size_t bufSize = m_height*screen->pitch;
    Uint8* tBuf = new Uint8[bufSize];
    const int xwt = 0.14285714;
    const int ywt = 0.07142857;
    for (unsigned x=1; x<m_width-1; x++) {
        for (unsigned y=1; y<m_height-1; y++) {
            Color res;
            Color c1,c2,c3,c4,c5,c6,c7,c8,c9;
            c1 = getPixel(x-1,y-1); c2 = getPixel(x,y-1);
            c3 = getPixel(x+1,y-1); c4 = getPixel(x+1,y);
            c5 = getPixel(x+1,y+1); c6 = getPixel(x,y+1);
            c7 = getPixel(x-1,y+1); c8 = getPixel(x-1,y);
            c9 = getPixel(x,y);
            res.blue=(char)(xwt*((double)c2.blue+c4.blue+c6.blue
                +c8.blue+c9.blue)+ywt*((double)c1.blue+c3.blue
                        +c5.blue+c7.blue));
            res.green=(char)(xwt*((double)c2.green+c4.green+c6.green
                +c8.green+c9.green)+ywt*((double)c1.green+c3.green
                        +c5.green+c7.green));
            res.red=(char)(xwt*((double)c2.red+c4.red+c6.red
                +c8.red+c9.red)+ywt*((double)c1.red+c3.red
                        +c5.red+c7.red));
            *(Uint32*)(tBuf + y*screen->pitch + x*4) =
                SDL_MapRGB(screen->format,res.red,res.green,res.blue);
        }
    }
    memcpy(screen->pixels,tBuf,bufSize);
}*/

