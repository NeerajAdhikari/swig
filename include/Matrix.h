#ifndef __MATRIX__
#define __MATRIX__

#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "ex.h"
#include "helper.h"
#include "containers.h"


// Matrix Class
class Matrix {
    private:
        float* m_matrix;
        unsigned m_row, m_col;
        unsigned m_space;

    public:

        // Returns an identity matrix of size nxn
        static Matrix identity(unsigned n);

        // Returns a zero matrix of size n,m
        static Matrix zero(const Pair<unsigned>& size);

        ~Matrix();

        // Constructor with size parameter
        Matrix(const Pair<unsigned>& size);

        // Copy constructor
        Matrix(const Matrix& m);

        // Assignment constructor
        void operator=(const Matrix& m);

        // Initialize the whole matrix using initializer list
        // Couldn't use variadic args because it
        // didn't support int and float at the same time
        template<typename... Types>
            void initialize(Types... args) {
                const int size = sizeof...(args);
                if(size!=space())
                    throw ex::DimensionMismatch();
                float dummy[] = { static_cast<float>(args)... };
                std::memcpy ( m_matrix, dummy , size*sizeof(float) );
            }

        // Returns the number of columns
        inline unsigned col() const {
            return m_col;
        }

        // Returns the number of rows of matrix
        inline unsigned row() const {
            return m_row;
        }

        // Returns the total space of matrix
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

        // Adds matrix m to itself
        // this = this + m
        void operator+=(const Matrix& m);

        // Subtracts matrix m to itself
        // this = this - m
        void operator-=(const Matrix& m);

        // Multiplies matrix m to itself
        // this = this * m
        void operator*=(const Matrix& m);

        // Multiplies itself to matrix m
        // this = m * this
        void operator/=(const Matrix& m);


        Matrix operator+(const Matrix& m) const;

        Matrix operator-(const Matrix& m) const;

        Matrix operator*(const Matrix& m) const;

        // Multiplies itself with a constant f
        // this = this * f
        void operator*=(float f);

        // Divides itself with a constant f
        // this = this / f
        void operator/=(float f);

        Matrix operator/(float f) const;

        Matrix operator*(float f) const;

        // Returns the transpose of a matrix
        // TODO inplace transpose
        Matrix transpose() const;

        // readjust itself to given size
        void readjust(const Pair<unsigned>& size);

        // adds a row to the matrix
        void addRow();

        // adds a col to the matrix
        void addColumn();

        // Returns true if it is a Square matrix
        bool isSquare() const;

        float determinant() const;

        // Prints the matrix
        void print() const ;
};

#endif
