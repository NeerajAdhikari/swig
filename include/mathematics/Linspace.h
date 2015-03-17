#ifndef __LINSPACE__
#define __LINSPACE__

#include "common/ex.h"
#include "common/helper.h"

class Linspace {
    private:
        // Difference in x and d;
        const int dx, dd;
        // current depth
        int d;
        // the counter
        int ctr;
        // get you the current position
        int pos;

        // start position
        int xs;
        int ds;
    public:

        // Start depth, End depth, Start pos, End pos
        Linspace(int dss, int dee, int xss, int xee);

        // Get the value of the Linspace
        operator int() const;

        // Increment the Linspace by "fwd"
        inline void operator+=(unsigned fwd);

        // Increment the Linspace by 1
        inline void operator++();

        inline int at(int a) const;
};


inline int Linspace::at(int i) const {
    if(dx==0)
        return ds;
    // Because the multiplication (i-xs)*dd exceeded int, I had a pretty bad time
    // Although the output will never exceed int
    return (long long)(i-xs)*dd/dx+ds;
}


// Start depth, End depth, no. of elements
inline Linspace::Linspace(int dss,int dee,int xss, int xee):
    dx(xee-xss),d(dss),dd(dee-dss),ctr(0),pos(0),xs(xss),ds(dss)
{
    if(dx<-1)
        throw ex::InitFailure();
}

// Get the value of the Linspace
inline Linspace::operator int() const {
    return d;
}

// Increment the Linspace by "fwd"
inline void Linspace::operator+=(unsigned fwd){
    if (dx==0)
        return;
    ctr += dd*fwd;
    if (Math::abs(ctr)>=dx){
        d += ctr/dx;
        ctr %= dx;
    }
    pos += fwd;
}

// Increment the Linspace by 1
inline void Linspace::operator++(){
    if (dx==0)
        return;
    ctr += dd;
    if (Math::abs(ctr)>=dx){
        d += ctr/dx;
        ctr %= dx;
    }
    pos++;
}

#endif
