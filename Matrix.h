#ifndef __MATRIX__
#define __MATRIX__

#include <iostream>
#include <iomanip>
#include <cmath>
#include "ex.h"
#include "helper.h"

// Matrix Class
class Matrix {
    private:
        float* m_matrix;
        unsigned m_row, m_col;
        unsigned m_space;
    public:

        ~Matrix();

        Matrix(unsigned row, unsigned col);

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

        void print();
};

#endif
