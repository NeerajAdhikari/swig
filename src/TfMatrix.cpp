#include "TfMatrix.h"

Matrix<float> TfMatrix::lookAt(const Vector& vrp, const Vector& vpn, const Vector& vup){
    Matrix<float> transformation({4,4});
    Vector forward = vpn.normalized();
    Vector side = (forward * vup).normalized();
    Vector up = (side * forward).normalized();

    // UVN system is left handed so forward is negative
    // Translate + Rotate
    transformation.initialize(
            side.x,     side.y,     side.z,     side % -vrp,
            up.x,       up.y,       up.z,       up % -vrp,
            -forward.x, -forward.y, -forward.z, -forward % -vrp,
            0,          0,          0,          1
            );
    return transformation;
}

Matrix<float> TfMatrix::toDevice(float width, float height, float maxDepth){
    // Converting normalized Z co-ordinate [-1,1] to depthmap [maxDepth,0]
    // Visible part is between n and f.
    // The Z map is shown below
    // space:        0       n       f
    // value: -ve   inf      1       0   -ve
    // NOTE: n and f have negative values
    // Normalized co-rodinate is in RHS
    // camera pointing towards negative Z axis
    Matrix<float> transformation({4,4});
    transformation.initialize(
            width,      0,          0,              width/2,
            0,          -height,    0,              height/2,
            0,          0,          -0.5*maxDepth,  0.5 * maxDepth,
            0,          0,          1,              0.0
            );
    return transformation;
}

Matrix<float> TfMatrix::perspective(float r, float t, float f, float n){
    Matrix<float> transformation({4,4});
    transformation.initialize(
            r/n,    0,      0,              0,
            0,      r/t,    0,              0,
            0,      0,      -(n+f)/(f-n),   -(2*f*n)/(f-n),
            0,      0,      -1.0,           0.0
            );
    return transformation;
}

Matrix<float> TfMatrix::perspective2(float ang, float ratio, float f, float n){
    // this matrix transfers world co-ordinates to homogenous coordinate and then normal-coordinates
    // NOTE: for near plane Z=-1 and far plane Z=1 after normalization LHS
    float tangent = std::tan( Math::toRadian(ang/2) );
    Matrix<float> transformation({4,4});
    transformation.initialize(
            1/tangent,  0,              0,              0,
            0,          ratio/tangent,  0,              0,
            0,          0,              -(n+f)/(f-n),   -(2*f*n)/(f-n),
            0,          0,              -1,              0
            );
    return transformation;
}

Matrix<float> TfMatrix::shearing(float a, float b, float c, float d, float e, float f){
    Matrix<float> transformation({4,4});
    transformation.initialize(
            1,  a,  b,  0,
            c,  1,  d,  0,
            e,  f,  1,  0,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix<float> TfMatrix::translation(const Vector& t){
    Matrix<float> transformation({4,4});
    transformation.initialize(
            1,  0,  0,  t.x,
            0,  1,  0,  t.y,
            0,  0,  1,  t.z,
            0,  0,  0,  1
            );
    return transformation;
}

Matrix<float> TfMatrix::scaling(const Vector& s, const Vector& p){
    Matrix<float> transformation({4,4});
    transformation.initialize(
            s.x,    0,      0,      (1-s.x)*p.x,
            0,      s.y,    0,      (1-s.y)*p.y,
            0,      0,      s.z,    (1-s.z)*p.z,
            0,      0,      0,      1
            );
    return transformation;
}

Matrix<float> TfMatrix::rotation(float radian, const Vector& r, const Vector& point){

    Vector axis = r.normalized();

    //degree = std::fmod(degree,360);
    float sine = std::sin(radian);
    float cosine = std::cos(radian);

    float u = axis.x, v = axis.y, w = axis.z;
    float a = point.x, b = point.y, c = point.z;

    Matrix<float> transformation({4,4});
    transformation.initialize(
            u*u+(v*v+w*w)*cosine,      u*v*(1-cosine)-w*sine,  u*w*(1-cosine)+v*sine,  (a*(v*v+w*w)-u*(b*v+c*w))*(1-cosine)+(b*w-c*v)*sine,
            u*v*(1-cosine)+w*sine,  v*v+(u*u+w*w)*cosine,      v*w*(1-cosine)-u*sine,  (b*(u*u+w*w)-v*(a*u+c*w))*(1-cosine)+(c*u-a*w)*sine,
            u*w*(1-cosine)-v*sine,  v*w*(1-cosine)+u*sine,  w*w+(u*u+v*v)*cosine,      (c*(u*u+v*v)-w*(a*u+b*v))*(1-cosine)+(a*v-b*u)*sine,
            0,                      0,                      0,                      1
            );
    return transformation;
}


Matrix<float> TfMatrix::rotationx(float alpha) {
    float cosine = std::cos(alpha);
    float sine  = std::sin(alpha);
    Matrix<float> transformation({4,4});
    transformation.initialize(
            1,      0,      0,      0,
            0,      cosine, -sine,  0,
            0,      sine,   cosine, 0,
            0,      0,      0,      1
            );
    return transformation;
}

Matrix<float> TfMatrix::rotationy(float beta) {
    float cosine = std::cos(beta);
    float sine  = std::sin(beta);
    Matrix<float> transformation({4,4});
    transformation.initialize(
            cosine, 0,      sine,   0,
            0,      1,      0,      0,
            -sine,  0,      cosine, 0,
            0,      0,      0,      1
            );
    return transformation;
}

Matrix<float> TfMatrix::rotationz(float gamma) {
    float cosine = std::cos(gamma);
    float sine  = std::sin(gamma);
    Matrix<float> transformation({4,4});
    transformation.initialize(
            cosine, -sine,  0,      0,
            sine,   cosine, 0,      0,
            0,      0,      1,      0,
            0,      0,      0,      1
            );
    return transformation;
}
