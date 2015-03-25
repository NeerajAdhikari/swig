#ifndef __COLOR_H__
#define __COLOR_H__

#include "mathematics/Vector.h"

struct Camera  {
    Vector vrp;
    Vector vpn;
    Vector vup;

    Camera (Vector rp, Vector pn, Vector up) {
        vrp = rp; vpn = pn; vup = up;
    }

    Camera() {
    }
};

#endif // __COLOR_H__
