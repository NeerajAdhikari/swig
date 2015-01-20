#ifndef __MATRIX_INT__
#define __MATRIX_INT__

#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "ex.h"
#include "helper.h"
#include "structures.h"
#include "containers.h"


// MatrixInt Class
class MatrixInt {
    private:
        uint32_t* m_matrix;
        unsigned m_row, m_col;
        unsigned m_space;

    public:
        ~MatrixInt();

        MatrixInt(const Pair<unsigned>& size);

        MatrixInt(const MatrixInt& m);

        void operator=(const MatrixInt& m);

        inline unsigned col() const {
            return m_col;
        }

        inline unsigned row() const {
            return m_row;
        }

        inline unsigned space() const {
            return m_space;
        }

        void addRow();
        void addColumn();
        void clear();

        // Getter in form matrix(x,y)
        inline uint32_t& operator()(unsigned row, unsigned col){
            if(row >= m_row || col >= m_col)
                throw ex::OutOfBounds();
            return *(m_matrix+row*m_col+col);
        }

        // Setter in form matrix(x,y)
        inline const uint32_t& operator()(unsigned row, unsigned col) const {
            if(row >= m_row || col >= m_col)
                throw ex::OutOfBounds();
            return *(m_matrix+row*m_col+col);
        }

        // Getter in the form matrix(p)
        inline const uint32_t& operator()(unsigned place) const {
            if(place >= space())
                throw ex::OutOfBounds();
            return *(m_matrix+place);
        }

        // Setter in the form matrix(p)
        inline uint32_t& operator()(unsigned place){
            if(place >= space())
                throw ex::OutOfBounds();
            return *(m_matrix+place);
        }

        void print() const ;
};

#endif
