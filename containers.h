#ifndef __CONTAINERS__
#define __CONTAINERS__

#include "iostream"
#include "ex.h"
#include "helper.h"

template <typename T>
struct Pair{

    T x,y;

    // Constructor
    Pair(T xx,T yy): x(xx), y(yy) {}

};

template <typename T>
struct Triplet{

    T x,y,z;

    // Constructor
    Triplet(T xx,T yy, T zz)
        : x(xx), y(yy), z(zz)
    {}

    void operator+=(const Triplet<T>& m){
        x += m.x;
        y += m.y;
        z += m.z;
    }

    void operator-=(const Triplet<T>& m){
        x -= m.x;
        y -= m.y;
        z -= m.z;
    }

    void operator*=(float m){
        x *= m;
        y *= m;
        z *= m;
    }

    void operator/=(float m){
        if(Math::equal(m,0))
            throw ex::DivideByZero();
        (*this) *= 1/m;
    }


    Triplet<T> operator+(const Triplet<T>& m) const {
        Triplet<T> temp = (*this);
            temp += m;
        return temp;
    }

    Triplet<T> operator-(const Triplet<T>& m) const {
        Triplet<T> temp = (*this);
            temp -= m;
        return temp;
    }


    Triplet<T> operator*(float m) const {
        Triplet<T> temp = (*this);
            temp *= m;
        return temp;
    }

    Triplet<T> operator/(float m) const {
        Triplet<T> temp = (*this);
            temp /= m;
        return temp;
    }

    // Display for debugging
    void display() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
    }
};

#endif
