#ifndef __HELPER__
#define __HELPER__
// helper.h contains all the helper functions
// related to mathematics, strings

//
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include <cmath>
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

inline void swap(float& a,float& b) {
    float t = a;
    a = b;
    b = t;
}

// trim from end
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}
#endif
