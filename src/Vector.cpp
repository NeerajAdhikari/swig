#include "Vector.h"

Vector::Vector():
    Quadruple<float>(0,0,0,0)
{
}

Vector::Vector(float xx, float yy, float zz, float ww):
    Quadruple<float>(xx,yy,zz,ww)
{
}

Vector::Vector(std::initializer_list<float> il)
        : Quadruple<float>(*il.begin(),*(il.begin()+1),*(il.begin()+2),*(il.begin()+3))
{}

Vector::Vector(const Vector& v)
    : Quadruple<float>(v.x,v.y,v.z,v.w)
{}

Vector Vector::normalized() const {
    float M = magnitude();
    if(Math::equal(x,0.0)&&Math::equal(y,0.0)&&Math::equal(z,0.0)) {
        return {0,0,0};
    }
    if (Math::equal(M,0.0))
        throw ex::DivideByZero();
    return {x/M,y/M,z/M,w};
}

float Vector::magnitude() const {
    // return std::sqrt(x % x);
    return std::sqrt(x*x+y*y+z*z);
}

float Vector::operator%(const Vector& m) const {
    return x*m.x+y*m.y+z*m.z;
}

Vector Vector::operator*(const Vector& m) const {
    Vector ans;
    ans.x = (this->y)*(m.z)-(this->z)*(m.y);
    ans.y = -((this->x)*(m.z)-(this->z)*(m.x));
    ans.z = (this->x)*(m.y)-(this->y)*(m.x);
    return ans;
}

void Vector::operator*=(const Vector& m) {
    (*this) = (*this)*m;
}

void Vector::operator/=(const Vector& m) {
    (*this) = m*(*this);
}
