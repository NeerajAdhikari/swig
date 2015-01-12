#include "TransformationMatrix.h"

Matrix TransformationMatrix::shearing(float a, float b, float c, float d, float e, float f){
    Matrix transformation({4,4});
    transformation.initialize(
            1,  a,  b,  0,
            c,  1,  d,  0,
            e,  f,  1,  0,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix TransformationMatrix::translation(const VectorTriplet& t){
    Matrix transformation({4,4});
    transformation.initialize(
            1,  0,  0,  t.x,
            0,  1,  0,  t.y,
            0,  0,  1,  t.z,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix TransformationMatrix::scaling(const VectorTriplet& s, const Vertex& p){
    Matrix transformation({4,4});
    transformation.initialize(
            s.x,    0,      0,      (1-s.x)*p.x,
            0,      s.y,    0,      (1-s.y)*p.y,
            0,      0,      s.z,    (1-s.z)*p.z,
            0,      0,      0,      1
            );
    return transformation;
}

Matrix TransformationMatrix::rotation(float degree, const VectorTriplet& r, const Vertex& point){
    //degree = std::fmod(degree,360);
    float radian = Math::toRadian(degree);
    float sine = std::sin(radian);
    float cosine = std::cos(radian);

    VectorTriplet axis = r.normalized();
    float u = axis.x;
    float v = axis.y;
    float w = axis.z;
    float a = point.x;
    float b = point.y;
    float c = point.z;
    float u2 = u*u;
    float v2 = v*v;
    float w2 = w*w;

    Matrix transformation({4,4});
    transformation.initialize(
        u2+(v2+w2)*cosine,      u*v*(1-cosine)-w*sine,  u*w*(1-cosine)+v*sine,  (a*(v2+w2)-u*(b*v+c*w))*(1-cosine)+(b*w-c*v)*sine,
        u*v*(1-cosine)+w*sine,  v2+(u2+w2)*cosine,      v*w*(1-cosine)-u*sine,  (b*(u2+w2)-v*(a*u+c*w))*(1-cosine)+(c*u-a*w)*sine,
        u*w*(1-cosine)-v*sine,  v*w*(1-cosine)+u*sine,  w2+(u2+v2)*cosine,      (c*(u2+v2)-w*(a*u+b*v))*(1-cosine)+(a*v-b*u)*sine,
        0,                      0,                      0,                      1
        );
    return transformation;
}
