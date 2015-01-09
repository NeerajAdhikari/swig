#ifndef __HELPER__
#define __HELPER__

#include "ex.h"
namespace Math {
    const float PRECISION = 0.0005;

    // Value of pi
    const float pi = 3.1415;

    // function to round a floating number
    inline int round(float a){
        return a>=0 ? a+0.5 : a-0.5;
    }

    inline bool equal(float a, float b){
        return  std::fabs(a-b) <= PRECISION;
    }

};

template <typename T=unsigned>
struct Pair{
    T x,y;
};

template <typename T=float>
struct Triplet{
    T x,y,z;

    // TODO normalized only meant to be used for floats;
    Triplet<T> normalized() const {
        T L = std::sqrt(x*x+y*y+z*z);
        if(Math::equal(L,0.0))
            throw ex::DivideByZero();
        return {x/L,y/L,z/L};
    }
};


#define Vertex Triplet<float>

#endif
