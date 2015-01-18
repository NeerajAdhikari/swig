#include "TfMatrix.h"

Matrix TfMatrix::shearing(float a, float b, float c, float d, float e, float f){
    Matrix transformation({4,4});
    transformation.initialize(
            1,  a,  b,  0,
            c,  1,  d,  0,
            e,  f,  1,  0,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix TfMatrix::translation(const VectorTriplet& t){
    Matrix transformation({4,4});
    transformation.initialize(
            1,  0,  0,  t.x,
            0,  1,  0,  t.y,
            0,  0,  1,  t.z,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix TfMatrix::scaling(const VectorTriplet& s, const Vertex& p){
    Matrix transformation({4,4});
    transformation.initialize(
            s.x,    0,      0,      (1-s.x)*p.x,
            0,      s.y,    0,      (1-s.y)*p.y,
            0,      0,      s.z,    (1-s.z)*p.z,
            0,      0,      0,      1
            );
    return transformation;
}

Matrix TfMatrix::rotation(float degree, const VectorTriplet& r, const Vertex& point){

    VectorTriplet axis = r.normalized();

    //degree = std::fmod(degree,360);
    float radian = Math::toRadian(degree);
    float sine = std::sin(radian);
    float cosine = std::cos(radian);

    float u = axis.x, v = axis.y, w = axis.z;
    float a = point.x, b = point.y, c = point.z;

    Matrix transformation({4,4});
    transformation.initialize(
        u*u+(v*v+w*w)*cosine,      u*v*(1-cosine)-w*sine,  u*w*(1-cosine)+v*sine,  (a*(v*v+w*w)-u*(b*v+c*w))*(1-cosine)+(b*w-c*v)*sine,
        u*v*(1-cosine)+w*sine,  v*v+(u*u+w*w)*cosine,      v*w*(1-cosine)-u*sine,  (b*(u*u+w*w)-v*(a*u+c*w))*(1-cosine)+(c*u-a*w)*sine,
        u*w*(1-cosine)-v*sine,  v*w*(1-cosine)+u*sine,  w*w+(u*u+v*v)*cosine,      (c*(u*u+v*v)-w*(a*u+b*v))*(1-cosine)+(a*v-b*u)*sine,
        0,                      0,                      0,                      1
        );
    return transformation;
}
