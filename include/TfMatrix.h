#ifndef __TRANSFORMATION_MATRIX__
#define __TRANSFORMATION_MATRIX__

#include "Vector.h"
#include "Matrix.h"

// TfMatrix class is derived from Matrix class
// and is meant for tasks related to 3d objects and transformation
class TfMatrix: public Matrix<float> {
    public:

        // Returns a shearing matrix
        static Matrix<float> shearing(float a, float b, float c, float d, float e, float f);

        // Returns a translation matrix by "translate"
        static Matrix<float> translation(const Vector& translate);

        // Returns a scaling matrix by "scale" about point "point"
        static Matrix<float> scaling(const Vector& scale,const Vector& point);

        // Returns a rotation matrix by and angle "degree" along direction vector "axis" passing through point "point"
        static Matrix<float> rotation(float degree, const Vector& axis, const Vector& point);

        // Returns a perspective projection
        // For a symmetric frustum where l = -r and b = -t
        // right top far near
        static Matrix<float> perspective(float r, float t, float f, float n);

        // Returns a perspective projection
        // POV, width to height ratio of view port
        static Matrix<float> perspective2(float POV, float ratio, float f, float n);

};

#endif
