#ifndef __HELPER__
#define __HELPER__

#include "ex.h"
namespace Math {

    // Value of pi
    const float pi = 3.1415;

    // function to round a floating number
    inline int round(float a){
        return a>=0 ? a+0.5 : a-0.5;
    }

};

#endif
