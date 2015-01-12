#ifndef __TRANSFORMATION_MATRIX__
#define __TRANSFORMATION_MATRIX__

#include "VectorTriplet.h"
#include "Matrix.h"

class TransformationMatrix: public Matrix {
    public:

        // Returns a shearing matrix
        static Matrix shearing(float a, float b, float c, float d, float e, float f);

        // Returns a translation matrix by "translate"
        static Matrix translation(const VectorTriplet& translate);

        // Returns a scaling matrix by "scale" about point "point"
        static Matrix scaling(const VectorTriplet& scale,const Vertex& point);

        // Returns a rotation matrix by and angle "degree" along direction vector "axis" passing through point "point"
        static Matrix rotation(float degree, const VectorTriplet& axis, const Vertex& point);

};

#endif
