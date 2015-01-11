#ifndef __HELPER__
#define __HELPER__

#include "ex.h"
namespace Math {
    const float PRECISION = 0.00005;

    // Value of pi
    const float pi = 3.14159;

    // function to round a floating number
    inline int round(float a){
        return a>=0 ? a+0.5 : a-0.5;
    }

    inline bool equal(float a, float b){
        return  std::fabs(a-b) <= PRECISION;
    }

    inline float toRadian(float degree){
        return degree/180*pi;
    }

    inline float toDegree(float radian){
        return radian/pi*180;
    }

};

#endif
