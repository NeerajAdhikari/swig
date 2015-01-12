#ifndef __STRUCTURES__
#define __STRUCTURES__

#include "containers.h"

struct Vertex : public Triplet<float> {

    // Constructor
    Vertex(float xx, float yy, float zz)
        : Triplet<float>(xx,yy,zz)
    {}

    // other properties

};

// A edge contains 2 index points
struct Edge :public Pair<unsigned> {

    // Constructor
    Edge(unsigned xx, unsigned yy)
        : Pair<unsigned>(xx,yy) {}

    // Some other attributes;
    // like line color, width

};

// A surface represents a triangle
// A surface contains 3 points
struct Surface : public Triplet<unsigned> {

    // Constructor
    Surface(unsigned xx, unsigned yy, unsigned zz)
        : Triplet<unsigned>(xx,yy,zz)
    {}

    // like color, luminosity, texture

};

#endif
