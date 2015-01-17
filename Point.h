#ifndef __POINT__
#define __POINT__

struct Color {
    char blue;
    char green;
    char red;
    char alpha;
};

struct ScreenPoint {
    Color color;
    unsigned x;
    unsigned y;
};

#endif
