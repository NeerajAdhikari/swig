#ifndef __VECTORTRIPLET__
#define __VECTORTRIPLET__

#include "containers.h"
#include "helper.h"
#include "ex.h"
#include <initializer_list>

struct Vector: public Quadruple<float> {

    Vector();

    float magnitude() const;

    // Constructor
    Vector(float xx, float yy, float zz, float ww);

    Vector(std::initializer_list<float> il);

    Vector(const Vector& v);

    // Return a normalized vector
    Vector normalized() const;

    // Dot Product
    float operator%(const Vector& m) const;

    // Cross Product
    Vector operator*(const Vector& m) const;
    // this x matrix
    void operator*=(const Vector& m);
    // matrix x this
    void operator/=(const Vector& m);

    friend float cosine(const Vector& a, const Vector& b){
        return (a % b)/(a.magnitude() * b.magnitude());
    }

    friend float sine(const Vector& a, const Vector& b){
        return (a * b).magnitude()/(a.magnitude() * b.magnitude());
    }

    void operator+=(const Vector& m){
        x += m.x;
        y += m.y;
        z += m.z;
    }

    void operator-=(const Vector& m){
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


    Vector operator+(const Vector& m) const {
        Vector temp = (*this);
            temp += m;
        return temp;
    }

    Vector operator-(const Vector& m) const {
        Vector temp = (*this);
            temp -= m;
        return temp;
    }


    Vector operator*(float m) const {
        Vector temp = (*this);
            temp *= m;
        return temp;
    }

    Vector operator/(float m) const {
        Vector temp = (*this);
            temp /= m;
        return temp;
    }

    // Display for debugging
    void display() const {
        std::cout << "(" << x << ", " << y << ", " << z << "," << w << ")" << std::endl;
    }
};


#endif
