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

        // Increment the Linspace by "fwd"
        inline void operator+=(unsigned fwd);

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
inline void Lincolor::operator+=(unsigned fwd){
    r += fwd;
    g += fwd;
    b += fwd;
}

// Increment the Linspace by 1
inline void Lincolor::operator++(){
    ++r;
    ++g;
    ++b;
}

#endif
