#ifndef __LINSPACE__
#define __LINSPACE__

#include "ex.h"
#include "Point.h"
#include "helper.h"
#include <iostream>

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
                throw ex::InitFailure();
        }

        // Get the value of the Linspace
        inline operator int() const {
            return d;
        }

        // Increment the Linspace by "fwd"
        inline void leap(unsigned fwd){
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



class Lincolor {
    private:
        // Difference in x and d;
        const int dx, dr,dg,db;
        // current depth
        int r,g,b;
        // the counter
        int ctr,ctg,ctb;
        // get you the current position
        int pos;
    public:

        // Start depth, End depth, no. of elements
        Lincolor(const Color& dss,const Color& dee,int dxx):
            dx(dxx-1),
            r(dss.red),g(dss.green),b(dss.blue),
            dr(dee.red-dss.red),dg(dee.green-dss.green),db(dee.blue-dss.blue),
            ctr(0),ctg(0),ctb(0),
            pos(0)
    {
        if(dx<-1)
            throw ex::InitFailure();
    }
        void display() const {
            std::cout << r << " " << g << " " << b << std::endl;
        }

        // Get the value of the Linspace
        inline operator Color() const {
            return {b,g,r,255};
        }

        // Increment the Linspace by "fwd"
        inline void leap(unsigned fwd){
            if (dx==0)
                return;
            ctr += dr*fwd;
            if (Math::abs(ctr)>=dx){
                r += ctr/dx;
                ctr %= dx;
            }
            ctg += dg*fwd;
            if (Math::abs(ctg)>=dx){
                g += ctg/dx;
                ctg %= dx;
            }
            ctb += db*fwd;
            if (Math::abs(ctb)>=dx){
                b += ctb/dx;
                ctb %= dx;
            }
            pos += fwd;
        }

        // Increment the Linspace by 1
        inline void operator++(){
            if (dx==0)
                return;
            ctr += dr;
            if (Math::abs(ctr)>=dx){
                r += ctr/dx;
                ctr %= dx;
            }
            ctg += dg;
            if (Math::abs(ctg)>=dx){
                g += ctg/dx;
                ctg %= dx;
            }
            ctb += db;
            if (Math::abs(ctb)>=dx){
                b += ctb/dx;
                ctb %= dx;
            }
            pos++;
        }
};


#endif
