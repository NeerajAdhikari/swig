#ifndef __COEFFECIENT__
#define __COEFFECIENT__

#include "Color.h"
#include "common/helper.h"
#include <iostream>

// RGB coeffecient
struct Coeffecient {
    public:
        float b,g,r;

    Coeffecient():b(0),g(0),r(0){
    }

    Coeffecient(float bb,float gg, float rr):b(bb),g(gg),r(rr){
    }

    Coeffecient operator*(const Coeffecient& c){
        return {b*c.b,g*c.g,r*c.r};
    }

    Coeffecient operator*(float c){
        return {b*c,g*c,r*c};
    }

    Coeffecient operator+(const Coeffecient& c){
        return {b+c.b,g+c.g,r+c.r};
    }

    void operator*=(const Coeffecient& c){
        b *= c.b;
        g *= c.g;
        r *= c.r;
    }

    void operator*=(float c){
        b *= c;
        g *= c;
        r *= c;
    }

    void operator+=(const Coeffecient& c){
        b += c.b;
        g += c.g;
        r += c.r;
    }

    // Get color from normalized Coeffecient
    operator Color(){
        return  {Math::min( b ,1.0f) * 255,
            Math::min( g ,1.0f) * 255,
            Math::min( r ,1.0f) * 255};
    }

    void print(){
        std::cout << b << " " << r << " " << g << std::endl;
    }
};

#endif
