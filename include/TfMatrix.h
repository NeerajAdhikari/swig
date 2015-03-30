#ifndef __TRANSFORMATION_MATRIX__
#define __TRANSFORMATION_MATRIX__

#include "mathematics/Vector.h"
#include "mathematics/Matrix.h"

// TfMatrix class is derived from Matrix class
// and is meant for tasks related to 3d objects and transformation
class TfMatrix: public Matrix<float> {
    public:

        // Returns a shearing matrix
        static Matrix<float> shearing(float a, float b,
                float c, float d, float e, float f);

        // Returns a translation matrix by "translate"
        static Matrix<float> translation(const Vector& translate);

        // Returns a scaling matrix by "scale" about point "point"
        static Matrix<float> scaling(const Vector& scale,
                const Vector& point);

        // Returns a rotation matrix by and angle "radian" along
        // direction vector "axis" passing through point "point"
        static Matrix<float> rotation(float radian,
                const Vector& axis, const Vector& point);

        // Returns a rotation matrix along X axis by angle of "alpha"
        static Matrix<float> rotationx(float alpha);

        // Returns a rotation matrix along Y axis by angle of "beta"
        static Matrix<float> rotationy(float beta);

        // Returns a rotation matrix along Z axis by angle of "gamma"
        static Matrix<float> rotationz(float gamma);

        // Returns a perspective projection
        // where right and top denotes the right and top boundaries of the projection plane
        // where far and near denotes the near farplane and nearplane positions along Z
        static Matrix<float> perspectivex(float right, float top, float far, float near);

        // Returns a perspective projection
        // FOV angle in degree, width to height ratio of projection plane
        static Matrix<float> perspective(float POV, float ratio, float far, float near);

        // Returns a camera transformation
        // View reference point, view plane normal, view up vector
        static Matrix<float> lookAt(const Vector& vrp, const Vector& vpn, const Vector& vup);

        // Returns a transformation from homogenous co-ordinate system
        // to the device co-ordinate system
        static Matrix<float> toDevice(float width, float height, float maxDepth);
};

#endif
