#ifndef __LINCOLOR__
#define __LINCOLOR__

#include "common/ex.h"
#include "common/helper.h"
#include "mathematics/Linspace.h"
#include "Color.h"

class Lincolor {
    private:
        Linspace b;
        Linspace g;
        Linspace r;
    public:

        // Start depth, End depth, no. of elements
        Lincolor(const Color& dss,const Color& dee,int dxx);

        // Get the value of the Linspace
        inline operator Color() const;

        Color valueAt(unsigned fwd) const {
            return {(uint8_t)b.valueAt(fwd),(uint8_t)g.valueAt(fwd),(uint8_t)r.valueAt(fwd),255};
        }

        // Increment the Linspace by "fwd"
        inline void leap(unsigned fwd);

        // Increment the Linspace by 1
        inline void operator++();
};


// Start depth, End depth, no. of elements
inline Lincolor::Lincolor(const Color& dss,const Color& dee,int dxx):
    b(dss.blue,dee.blue,dxx),
    r(dss.red,dee.red,dxx),
    g(dss.green,dee.green,dxx)
{
}

// Get the value of the Linspace
inline Lincolor::operator Color() const {
    return {(uint8_t)b,(uint8_t)g,(uint8_t)r,255};
}

// Increment the Linspace by "fwd"
inline void Lincolor::leap(unsigned fwd){
    r.leap(fwd);
    g.leap(fwd);
    b.leap(fwd);
}

// Increment the Linspace by 1
inline void Lincolor::operator++(){
    ++r;
    ++g;
    ++b;
}

#endif
