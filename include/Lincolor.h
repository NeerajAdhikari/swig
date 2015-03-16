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


        // Start depth, End depth, no. of elements
        Lincolor(const Color& dss,const Color& dee,int xss, int xee);

        // Get the value of the Linspace
        inline operator Color() const;

        // Increment the Linspace by "fwd"
        inline void operator+=(unsigned fwd);

        // Increment the Linspace by 1
        inline void operator++();

        inline Color at(int i) const;
};

inline Color Lincolor::at(int i) const {
    return { (uint8_t)b.at(i), (uint8_t)g.at(i), (uint8_t)r.at(i),255 };
}


inline Lincolor::Lincolor(const Color& dss,const Color& dee,int xss, int xee):
    b(dss.blue,dee.blue,xss,xee),
    r(dss.red,dee.red,xss,xee),
    g(dss.green,dee.green,xss,xee)
{

}

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
