#ifndef __VECTORTRIPLET__
#define __VECTORTRIPLET__

#include "containers.h"
#include "helper.h"
#include "ex.h"

struct VectorTriplet: public Triplet<float> {

    VectorTriplet();

    // Constructor
    VectorTriplet(float xx, float yy, float zz);

    // Return a normalized vector
    VectorTriplet normalized() const;

    // Dot Product
    void operator%=(const VectorTriplet& m);
    VectorTriplet operator%(const VectorTriplet& m) const;

    // Cross Product
    VectorTriplet operator*(const VectorTriplet& m) const;
    // this x matrix
    void operator*=(const VectorTriplet& m);
    // matrix x this
    void operator/=(const VectorTriplet& m);

};


#endif
