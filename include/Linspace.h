#ifndef __LINSPACE__
#define __LINSPACE__

#include "ex.h"
#include "helper.h"

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
        Linspace(int dss,int dee,int dxx)
            :dx(dxx-1),d(dss),dd(dee-dss),ctr(0),pos(0)
        {
            if(dx<-1)
                throw "dxx must be atleast 0";
        }

        inline operator int(){
            return d;
        }

        // Forward to certain position
        void leap(unsigned fwd){
            if (dx==0)
                return;
            ctr += dd*fwd;
            if (Math::abs(ctr)>=dx){
                d += ctr/dx;
                ctr %= dx;
            }
            pos += fwd;
        }

        inline void operator++(){
            if (dx==0)
                return;
            ctr += dd;
            if (Math::abs(ctr)>=dx){
                d += ctr/dx;
                ctr %= dx;
            }
            pos++;
        }
};
#endif
