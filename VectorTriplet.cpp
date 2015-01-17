#include "VectorTriplet.h"
#include "structures.h"

VectorTriplet::VectorTriplet():
    Triplet<float>(0,0,0)
{
}

VectorTriplet::VectorTriplet(float xx, float yy, float zz):
    Triplet<float>(xx,yy,zz)
{
}

VectorTriplet::VectorTriplet(std::initializer_list<float> il)
        : Triplet<float>(*il.begin(),*(il.begin()+1),*(il.begin()+2))
{}

VectorTriplet::VectorTriplet(const Vertex& v)
    : Triplet<float>(v.x,v.y,v.z)
{}

VectorTriplet::VectorTriplet(const Triplet<float>& v)
    : Triplet<float>(v.x,v.y,v.z)
{}

VectorTriplet VectorTriplet::normalized() const {
    float M = magnitude();
    if (Math::equal(M,0.0))
        throw ex::DivideByZero();
    return {x/M,y/M,z/M};
}

float VectorTriplet::magnitude() const {
    // return std::sqrt(x % x);
    return std::sqrt(x*x+y*y+z*z);
}

float VectorTriplet::operator%(const VectorTriplet& m) const {
    return x*m.x+y*m.y+z*m.z;
}

VectorTriplet VectorTriplet::operator*(const VectorTriplet& m) const {
    VectorTriplet ans;
    ans.x = (this->y)*(m.z)-(this->z)*(m.y);
    ans.y = -((this->x)*(m.z)-(this->z)*(m.x));
    ans.z = (this->x)*(m.y)-(this->y)*(m.x);
    return ans;
}

void VectorTriplet::operator*=(const VectorTriplet& m) {
    (*this) = (*this)*m;
}

void VectorTriplet::operator/=(const VectorTriplet& m) {
    (*this) = m*(*this);
}
