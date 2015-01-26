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

    public:

        // Start depth, End depth, no. of elements
        Linspace(int dss,int dee,int dxx);

        // Get the value of the Linspace
        operator int() const;

        int valueAt(unsigned fwd) const {
            if (dx==0)
                return 0;
            int tctr = dd*fwd;
            int td;
            if (Math::abs(tctr)>=dx){
                td = tctr/dx;
                //tctr %= dx;
            }
            //pos += fwd;
            return td;
        }

        // Increment the Linspace by "fwd"
        inline void leap(unsigned fwd);

        // Increment the Linspace by 1
        inline void operator++();
};

// Start depth, End depth, no. of elements
inline Linspace::Linspace(int dss,int dee,int dxx):
    dx(dxx-1),d(dss),dd(dee-dss),ctr(0),pos(0)
{
    if(dx<-1)
        throw ex::InitFailure();
}

// Get the value of the Linspace
inline Linspace::operator int() const {
    return d;
}

// Increment the Linspace by "fwd"
inline void Linspace::leap(unsigned fwd){
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
