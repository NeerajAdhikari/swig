#ifndef __VECTORTRIPLET__
#define __VECTORTRIPLET__

#include "containers.h"
#include "helper.h"
#include "ex.h"
#include <initializer_list>

class Vertex;

struct VectorTriplet: public Triplet<float> {

    VectorTriplet();

    float magnitude() const;

    // Constructor
    VectorTriplet(float xx, float yy, float zz);

    VectorTriplet(std::initializer_list<float> il);

    VectorTriplet(const Vertex& v);

    VectorTriplet(const Triplet<float>& v);

    // Return a normalized vector
    VectorTriplet normalized() const;

    // Dot Product
    float operator%(const VectorTriplet& m) const;

    // Cross Product
    VectorTriplet operator*(const VectorTriplet& m) const;
    // this x matrix
    void operator*=(const VectorTriplet& m);
    // matrix x this
    void operator/=(const VectorTriplet& m);

    friend float cosine(const VectorTriplet& a, const VectorTriplet& b){
        return (a % b)/(a.magnitude() * b.magnitude());
    }

    friend float sine(const VectorTriplet& a, const VectorTriplet& b){
        return (a * b).magnitude()/(a.magnitude() * b.magnitude());
    }

};


#endif
