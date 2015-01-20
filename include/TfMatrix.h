#ifndef __TRANSFORMATION_MATRIX__
#define __TRANSFORMATION_MATRIX__

#include "Vector.h"
#include "Matrix.h"

// TfMatrix class is derived from Matrix class
// and is meant for tasks related to 3d objects and transformation
class TfMatrix: public Matrix {
    public:

        // Returns a shearing matrix
        static Matrix shearing(float a, float b, float c, float d, float e, float f);

        // Returns a translation matrix by "translate"
        static Matrix translation(const Vector& translate);

        // Returns a scaling matrix by "scale" about point "point"
        static Matrix scaling(const Vector& scale,const Vector& point);

        // Returns a rotation matrix by and angle "degree" along direction vector "axis" passing through point "point"
        static Matrix rotation(float degree, const Vector& axis, const Vector& point);

};

#endif
