#ifndef __COEFFECIENT__
#define __COEFFECIENT__

#include <iostream>

// RGB coeffecient
struct Coeffecient {
    public:
        float b,g,r;

    void display(){
        std::cout << b << " " << g << " " << r <<std::endl;
    }

};

#endif
