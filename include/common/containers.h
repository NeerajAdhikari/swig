#ifndef __CONTAINERS__
#define __CONTAINERS__
// container.h contains data structures
// for grouped data

template <typename T>
struct Pair{
    T x,y;

    // Constructor
    Pair(T xx,T yy): x(xx), y(yy) {}

};

template<typename T>
struct Triplet {
    T x,y,z;

    // Constructor
    Triplet(T xx,T yy, T zz)
        : x(xx), y(yy), z(zz)
    {}
};

template <typename T>
struct Quadruple{
    T x,y,z,w;

    // Constructor
    Quadruple(T xx,T yy, T zz, T ww)
        : x(xx), y(yy), z(zz), w(ww)
    {}
};

#endif
