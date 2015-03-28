#ifndef __LINSPACEF__
#define __LINSPACEF__

#include "common/ex.h"
#include "common/helper.h"

class LinspaceF {
    private:
        // Difference in x and d;
        const int dx;
        const double dd;
        // current depth
        double d;
        // the counter
        int ctr;
        // get you the current position
        int pos;

        // start position
        int xs;
        double ds;
    public:

        // Start depth, End depth, Start pos, End pos
        LinspaceF(double dss, double dee, int xss, int xee);

        // Get the value of the LinspaceF
        operator double() const;

        // Increment the LinspaceF by "fwd"
        inline void operator+=(unsigned fwd);

        // Increment the LinspaceF by 1
        inline void operator++();

        inline double at(int a) const;
};


inline double LinspaceF::at(int i) const {
    if(dx==0)
        return ds;
    // Because the multiplication (i-xs)*dd exceeded int,
    // I had a pretty bad time
    // Although the output will never exceed int
    int i_d = i-xs;
    long double f = dd/dx;
    return (i_d*f)+ds;
}


// Start depth, End depth, no. of elements
inline LinspaceF::LinspaceF(double dss,double dee,int xss, int xee):
    dx(xee-xss),d(dss),dd(dee-dss),ctr(0),pos(0),xs(xss),ds(dss)
{
    if(dx<-1)
        throw ex::InitFailure();
}

// Get the value of the LinspaceF
inline LinspaceF::operator double() const {
    return d;
}

// Increment the LinspaceF by "fwd"
inline void LinspaceF::operator+=(unsigned fwd){
    if (dx==0)
        return;
    ctr += dd*fwd;
    if (Math::abs(ctr)>=dx){
        d += ctr/dx;
        ctr %= dx;
    }
    pos += fwd;
}

// Increment the LinspaceF by 1
inline void LinspaceF::operator++(){
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
