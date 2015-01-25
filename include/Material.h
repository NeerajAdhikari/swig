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

    Material():
        ka({0.01,0.01,0.01}),
        kd({0.05,0.05,0.05}),
        ks({0.05,0.05,0.05}),
        ns(10)
    {
    }
};
