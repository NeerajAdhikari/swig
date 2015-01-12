#include "VectorTriplet.h"

VectorTriplet::VectorTriplet():
    Triplet<float>(0,0,0)
{
}

VectorTriplet::VectorTriplet(float xx, float yy, float zz):
    Triplet<float>(xx,yy,zz)
{
}

VectorTriplet VectorTriplet::normalized() const {
    float L = std::sqrt(x*x+y*y+z*z);
    if (Math::equal(L,0.0))
        throw ex::DivideByZero();
    return {x/L,y/L,z/L};
}

void VectorTriplet::operator%=(const VectorTriplet& m){
    x *= m.x;
    y *= m.y;
    z *= m.z;
}

VectorTriplet VectorTriplet::operator%(const VectorTriplet& m) const {
    VectorTriplet ans = (*this);
    ans %= m;
    return ans;
}

VectorTriplet VectorTriplet::operator*(const VectorTriplet& m) const {
    VectorTriplet ans;
    ans.x = (this->y)*(m.z)-(this->z)*(m.y);
    ans.y = -(this->x)*(m.z)-(this->z)*(m.x);
    ans.z = (this->y)*(m.x)-(this->x)*(m.y);
    return ans;
}

void VectorTriplet::operator*=(const VectorTriplet& m) {
    (*this) = (*this)*m;
}

void VectorTriplet::operator/=(const VectorTriplet& m) {
    (*this) = m*(*this);
}
