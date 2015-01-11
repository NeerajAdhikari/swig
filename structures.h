#ifndef __STRUCTURES__
#define __STRUCTURES__

#include "ex.h"
#include "helper.h"

template <typename T=unsigned>
struct Pair{

    T x,y;

    Pair(T xx,T yy): x(xx), y(yy) {}

};

template <typename T=float>
struct Triplet{

    T x,y,z;

    Triplet(T xx,T yy, T zz)
        : x(xx), y(yy), z(zz)
    {}

};

struct Vertex : public Triplet<float> {

    Vertex(float xx, float yy, float zz)
        : Triplet<float>(xx,yy,zz)
    {}

    // other properties

};

// A edge contains 2 index points
struct Edge :public Pair<unsigned> {

    Edge(unsigned xx, unsigned yy)
        : Pair<unsigned>(xx,yy) {}
    // Some other attributes;
    // like line color, width

};

// A surface represents a triangle
// A surface contains 3 points
struct Surface : public Triplet<unsigned> {

    Surface(unsigned xx, unsigned yy, unsigned zz)
        : Triplet<unsigned>(xx,yy,zz)
    {}

    // like color, luminosity, texture

};

struct VectorTriplet: public Triplet<float> {

    VectorTriplet(float xx, float yy, float zz)
        : Triplet<float>(xx,yy,zz)
    {}

    VectorTriplet normalized() const {
        float L = std::sqrt(x*x+y*y+z*z);
        if (Math::equal(L,0.0))
            throw ex::DivideByZero();
        return {x/L,y/L,z/L};
    }

};

#endif
