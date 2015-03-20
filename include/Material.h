#ifndef __MATERIAL__
#define __MATERIAL__
#include "Coeffecient.h"

struct Material {
    // ambient-reflection coefficient
    Coeffecient ka;
    // diffuse-reflection coefficient
    Coeffecient kd;
    // specular-reflection coefficient
    Coeffecient ks;
    // specular-reflection parameter (smaller values for dull surfaces)
    float ns;

    Material(const Coeffecient& a, const Coeffecient& d, const Coeffecient& s,float n):
        ka(a),
        kd(d),
        ks(s),
        ns(n)
    {
    }

    void print(){
        ka.print();
        kd.print();
        ks.print();
        std::cout << ns << std::endl;
    }

};
#endif
