#ifndef __MATRIX__
#define __MATRIX__

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include "structures.h"
#include "ex.h"
#include "helper.h"

// LEFT
// Transpose
// Inverse
// Determinant

// TODO
// projection
// skew
// TODO triangularization of a surface
// TODO load objects from file;
//      so total no. of edges and surfaces are always known
//      hence use arrays as well
// NOTE
// Random Access on lists is slower and
// the insertion is almost always at the start of the program
// so we could use vector to facilitate random access
// TODO
// a vertex may contain color information

// vertex normal may change in rotation
// and shearing`

// Matrix Class
class Matrix {
    private:
        float* m_matrix;
        unsigned m_row, m_col;
        unsigned m_space;

    public:

        // Returns a shearing matrix
        static Matrix shearing(float a, float b, float c, float d, float e, float f);

        // Returns a translation matrix by "translate"
        static Matrix translation(const VectorTriplet& translate);

        // Returns a scaling matrix by "scale" about point "point"
        static Matrix scaling(const VectorTriplet& scale,const Vertex& point);

        // Returns a rotation matrix by and angle "degree" along direction vector "axis" passing through point "point"
        static Matrix rotation(float degree, const VectorTriplet& axis, const Vertex& point);


        // Returns an identity matrix of size nxn
        static Matrix identity(unsigned n);

        // Returns a zero matrix of size n,m
        static Matrix zero(const Pair<unsigned>& size);


        ~Matrix();

        Matrix(const Pair<unsigned>& size);

        Matrix(const Matrix& m);

        void operator=(const Matrix& m);

        inline unsigned col() const {
            return m_col;
        }

        inline unsigned row() const {
            return m_row;
        }

        inline unsigned space() const {
            return m_space;
        }

        // Getter in form matrix(x,y)
        inline float& operator()(unsigned row, unsigned col){
            if(row >= m_row || col >= m_col)
                throw ex::OutOfBounds();
            return *(m_matrix+row*m_col+col);
        }

        // Setter in form matrix(x,y)
        inline const float& operator()(unsigned row, unsigned col) const {
            if(row >= m_row || col >= m_col)
                throw ex::OutOfBounds();
            return *(m_matrix+row*m_col+col);
        }

        // Getter in the form matrix(p)
        inline const float& operator()(unsigned place) const {
            if(place >= space())
                throw ex::OutOfBounds();
            return *(m_matrix+place);
        }

        // Setter in the form matrix(p)
        inline float& operator()(unsigned place){
            if(place >= space())
                throw ex::OutOfBounds();
            return *(m_matrix+place);
        }

        // Initialize the whole matrix
        // Some deep shit initializer this is
        // Couldn't use variadic args because it
        // didn't support int and float at the same time
        template<typename... Types>
            void initialize(Types... args) {
                const int size = sizeof...(args);
                if(size!=space())
                    throw ex::DimensionMismatch();
                float dummy[] = { static_cast<float>(args)... };
                // memcpy performed for efficiency
                std::memcpy ( m_matrix, dummy , size*sizeof(float) );
            }

        Matrix operator+(const Matrix& m) const;

        void operator+=(const Matrix& m);

        Matrix operator-(const Matrix& m) const;

        void operator-=(const Matrix& m);

        Matrix operator*(const Matrix& m) const;

        // this x m
        void operator*=(const Matrix& m);

        // m x this
        void operator%=(const Matrix& m);

        Matrix operator*(float f) const;

        void operator*=(float f);

        Matrix operator/(float f) const;

        void operator/=(float f);

        void print() const ;
};

#endif
