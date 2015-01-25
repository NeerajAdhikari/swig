#ifndef __VECTORTRIPLET__
#define __VECTORTRIPLET__

#include "containers.h"
#include "helper.h"
#include "ex.h"

class Vector: public Quadruple<float> {
    public:
        Vector();

        // Constructor
        Vector(float xx, float yy, float zz, float ww=0);

        Vector(const Vector& v);

        // Returns the magnitude of the Vector
        float magnitude() const;

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

        void operator+=(const Vector& m);

        void operator-=(const Vector& m);

        void operator*=(float m);

        void operator/=(float m);

        Vector operator+(const Vector& m) const ;

        Vector operator-(const Vector& m) const ;

        Vector operator*(float m) const ;

        Vector operator/(float m) const ;

        // Display for debugging
        void display() const ;

        // Returns cosine of angle between two vectors
        static float cosine(const Vector& a, const Vector& b){
            return (a % b)/(a.magnitude() * b.magnitude());
        }

        // Returns sine of angle between two vectors
        static float sine(const Vector& a, const Vector& b){
            return (a * b).magnitude()/(a.magnitude() * b.magnitude());
        }
};

inline Vector::Vector():
    Quadruple<float>(0,0,0,0)
{
}

inline Vector::Vector(float xx, float yy, float zz, float ww):
    Quadruple<float>(xx,yy,zz,ww)
{
}

inline Vector::Vector(const Vector& v):
    Quadruple<float>(v.x,v.y,v.z,v.w)
{}

inline Vector Vector::normalized() const {
    float M = magnitude();
    if (Math::equal(M,0.0))
        throw ex::DivideByZero();
    return {x/M,y/M,z/M,w};
}

inline float Vector::magnitude() const {
    // return std::sqrt(x % x);
    return std::sqrt(x*x+y*y+z*z);
}

inline float Vector::operator%(const Vector& m) const {
    return x*m.x+y*m.y+z*m.z;
}

inline Vector Vector::operator*(const Vector& m) const {
    Vector ans;
    ans.x = (this->y)*(m.z)-(this->z)*(m.y);
    ans.y = -((this->x)*(m.z)-(this->z)*(m.x));
    ans.z = (this->x)*(m.y)-(this->y)*(m.x);
    return ans;
}

inline void Vector::operator*=(const Vector& m) {
    (*this) = (*this)*m;
}

inline void Vector::operator/=(const Vector& m) {
    (*this) = m*(*this);
}

inline void Vector::operator+=(const Vector& m){
    x += m.x;
    y += m.y;
    z += m.z;
}

inline void Vector::operator-=(const Vector& m){
    x -= m.x;
    y -= m.y;
    z -= m.z;
}

inline void Vector::operator*=(float m){
    x *= m;
    y *= m;
    z *= m;
}

inline void Vector::operator/=(float m){
    if(Math::equal(m,0))
        throw ex::DivideByZero();
    (*this) *= 1/m;
}


inline Vector Vector::operator+(const Vector& m) const {
    Vector temp = (*this);
    temp += m;
    return temp;
}

inline Vector Vector::operator-(const Vector& m) const {
    Vector temp = (*this);
    temp -= m;
    return temp;
}


inline Vector Vector::operator*(float m) const {
    Vector temp = (*this);
    temp *= m;
    return temp;
}

inline Vector Vector::operator/(float m) const {
    Vector temp = (*this);
    temp /= m;
    return temp;
}

// Display for debugging
inline void Vector::display() const {
    std::cout << "(" << x << ", " << y << ", " << z << "," << w << ")" << std::endl;
}

#endif
