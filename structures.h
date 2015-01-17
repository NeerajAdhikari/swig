#ifndef __STRUCTURES__
#define __STRUCTURES__
// structures.h contains data structures
// related to 3d coordinate objects

#include "containers.h"
#include "VectorTriplet.h"
#include <initializer_list>

struct Vertex : public Triplet<float> {

    // Constructor
    Vertex(float xx, float yy, float zz)
        : Triplet<float>(xx,yy,zz)
    {}

    // Constructor from an initializer list
    Vertex(std::initializer_list<float> il)
        : Triplet<float>(*il.begin(),*(il.begin()+1),*(il.begin()+2))
    {}

    // other properties

};

// A edge contains 2 index points
struct Edge :public Pair<unsigned> {

    // Constructor
    Edge(unsigned xx, unsigned yy)
        : Pair<unsigned>(xx,yy) {}

    Edge(std::initializer_list<unsigned> il)
        : Pair<unsigned>(*il.begin(),*(il.begin()+1))
    {}

    // Some other attributes;
    // like line color, width
};

// A surface represents a triangle
// A surface contains 3 points
struct Surface : public Triplet<unsigned> {

    VectorTriplet normal;

    // Constructor
    Surface(unsigned xx, unsigned yy, unsigned zz)
        : Triplet<unsigned>(xx,yy,zz)
    {}

    Surface(std::initializer_list<unsigned> l)
        : Triplet<unsigned>(*l.begin(),*(l.begin()+1),*(l.begin()+2))
    {}

    // like color, luminosity, texture

};

#endif
