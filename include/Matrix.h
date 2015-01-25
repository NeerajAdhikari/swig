#ifndef __MATRIX__
#define __MATRIX__

#include <cstring>
#include <iostream>
#include <iomanip>

#include "ex.h"
#include "containers.h"
#include "helper.h"

// Matrix Class
template <class T=float>
class Matrix {
    private:
        T* m_matrix;
        unsigned m_row, m_col;
        unsigned m_space;

    public:

        // Returns an identity matrix of size nxn
        static Matrix<T> identity(unsigned n);

        // Returns a zero matrix of size n,m
        static Matrix<T> zero(const Pair<unsigned>& size);

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
                T dummy[] = { static_cast<T>(args)... };
                std::memcpy ( m_matrix, dummy , size*sizeof(T) );
            }

        // Returns the number of columns
        inline unsigned col() const;

        // Returns the number of rows of matrix
        inline unsigned row() const;

        // Returns the total space of matrix
        inline unsigned space() const;

        // Getter in form matrix(x,y)
        T& operator()(unsigned row, unsigned col);

        // Setter in form matrix(x,y)
        const T& operator()(unsigned row, unsigned col) const;

        // Getter in the form matrix(p)
        const T& operator()(unsigned place) const;

        // Setter in the form matrix(p)
        T& operator()(unsigned place);

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


        Matrix<T> operator+(const Matrix& m) const;

        Matrix<T> operator-(const Matrix& m) const;

        Matrix<T> operator*(const Matrix& m) const;

        // Multiplies itself with a constant f
        // this = this * f
        void operator*=(T f);

        // Divides itself with a constant f
        // this = this / f
        void operator/=(T f);

        Matrix<T> operator/(T f) const;

        Matrix<T> operator*(T f) const;

        // Returns the transpose of a matrix
        // TODO inplace transpose
        Matrix<T> transpose() const;

        // readjust itself to given size
        void readjust(const Pair<unsigned>& size);

        // adds a row to the matrix
        void addRow();

        // adds a col to the matrix
        void addColumn();

        // Returns true if it is a Square matrix
        bool isSquare() const;

        // Returns the determinant of a matrix
        T determinant() const;

        // Clears the matrix
        void clear();

        // Prints the matrix
        void print() const ;
};


// Getter in form matrix(x,y)
template<class T>
inline T& Matrix<T>::operator()(unsigned row, unsigned col){
    if(row >= m_row || col >= m_col)
        throw ex::OutOfBounds();
    return *(m_matrix+row*m_col+col);
}

// Setter in form matrix(x,y)
template<class T>
inline const T& Matrix<T>::operator()(unsigned row, unsigned col) const {
    if(row >= m_row || col >= m_col)
        throw ex::OutOfBounds();
    return *(m_matrix+row*m_col+col);
}

// Getter in the form matrix(p)
template<class T>
inline const T& Matrix<T>::operator()(unsigned place) const {
    if(place >= space())
        throw ex::OutOfBounds();
    return *(m_matrix+place);
}

// Setter in the form matrix(p)
template<class T>
inline T& Matrix<T>::operator()(unsigned place){
    if(place >= space())
        throw ex::OutOfBounds();
    return *(m_matrix+place);
}



// Returns the number of columns
template<class T>
inline unsigned Matrix<T>::col() const {
    return m_col;
}

// Returns the number of rows of matrix
template<class T>
inline unsigned Matrix<T>::row() const {
    return m_row;
}

// Returns the total space of matrix
template<class T>
inline unsigned Matrix<T>::space() const {
    return m_space;
}


template<class T>
inline void Matrix<T>::clear() {
    memset(m_matrix,0,space()*sizeof(T));
}

template<class T>
bool Matrix<T>::isSquare() const {
    return row() == col();
}

template<class T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix<T> t({col(),row()});
    for(int i=0;i<row();i++)
        for(int j=0;j<col();j++)
            t(j,i) = (*this)(i,j);
    return t;
}

template<class T>
void Matrix<T>::addRow() {
    readjust({m_row+1,m_col});
}

template<class T>
void Matrix<T>::addColumn() {
    readjust({m_row,m_col+1});
}

template<class T>
void Matrix<T>::readjust(const Pair<unsigned>& size) {

    unsigned tspace = size.x * size.y;
    T* tmatrix = new T[tspace];

    if (size.y == m_col && size.x == m_row){
        return;
    } else if(size.y == m_col) {
        // If column number is equal then a block copy can be done
        unsigned smallers = tspace < space() ? tspace : space();
        std::memcpy( tmatrix, m_matrix, smallers * sizeof(T));
    } else {
        // If column number varies then a linear copy must be done
        unsigned smallerx = size.x < row() ? size.x : row();
        unsigned smallery = size.y < col() ? size.y : col();
        for(int i = 0;i<smallerx;i++)
            std::memcpy( tmatrix+i*size.y , m_matrix+i*col(), smallery* sizeof(T));
    }

    delete []m_matrix;

    m_matrix = tmatrix;
    m_row = size.x;
    m_col = size.y;
    m_space = tspace;
}

template<class T>
T Matrix<T>::determinant() const {
    if(row()!=col())
        throw ex::DimensionMismatch();

    Matrix<T> arr = *this;
    unsigned size = row();
    T det = 1;

    for(int i=0;i<size;i++){  // Columns

        // If any zero in diagonal, push it downward
        if( Math::equal(arr(i,i),0) ) {
            for(int j=i+1;j<size;j++) {
                if( Math::equal(arr(j,i),0) )
                    continue;
                for(int k=0;k<size;k++)
                    swap(arr(i,k),arr(j,k));
                det *= std::pow(-1, (j-i) + (j-i-1));
                break;
            }
            if( Math::equal(arr(i,i),0) )
                throw ex::DivideByZero();
            // If not breaked till now
        }

        // Calculate upper Triangular Matrix
        for(int j=i+1;j<size;j++){          // Row
            float c = arr(j,i)/arr(i,i);
            for(int k=0;k<size;k++)       // Columns+1
                arr(j,k) -= c*arr(i,k);
        }

        det *= arr(i,i);
    }

    return det;
}


template<class T>
Matrix<T> Matrix<T>::identity(unsigned n) {
    Matrix<T> inst({n,n});
    for(int i=0;i<inst.space();i++)
        inst(i) = (i%(n+1) == 0);
    return inst;
}

template<class T>
Matrix<T> Matrix<T>::zero(const Pair<unsigned>& size){
    Matrix<T> inst({size.x,size.y});
    for(int i=0;i<inst.space();i++)
        inst(i) = 0;
    return inst;
}

template<class T>
Matrix<T>::~Matrix(){
    delete []m_matrix;
}

template<class T>
Matrix<T>::Matrix(const Pair<unsigned>& size) :
    m_row(size.x), m_col(size.y), m_space(size.x*size.y)
{
    m_matrix  = new T[space()];
}

template<class T>
Matrix<T>::Matrix(const Matrix& m){

    m_matrix = new T[m.space()];

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();

    std::memcpy ( m_matrix, m.m_matrix , m_space*sizeof(T) );

}


template<class T>
void Matrix<T>::operator=(const Matrix& m){
    // Assigning itself to itself
    if( this == &m )
        return;

    // If the space is equivalent // then things can be easily overwritten
    if(space() != m.space()){
        delete []m_matrix;
        m_matrix = new T[m.space()];
    }

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();

    std::memcpy ( m_matrix, m.m_matrix , m_space*sizeof(T) );
}

template<class T>
Matrix<T> Matrix<T>::operator+(const Matrix& m) const {
    Matrix<T> n(*this);
    n+=m;
    return n;
}

template<class T>
void Matrix<T>::operator+=(const Matrix& m) {
    if( m.row() != row() || m.col() != col())
        throw ex::DimensionMismatch();
    for(int i=0;i<space();i++)
        (*this)(i) += m(i);
}

template<class T>
Matrix<T> Matrix<T>::operator-(const Matrix& m) const {
    Matrix<T> n(*this);
    n-=m;
    return n;
}

template<class T>
void Matrix<T>::operator-=(const Matrix& m) {
    if( m.row() != row() || m.col() != col())
        throw ex::DimensionMismatch();
    for(int i=0;i<space();i++)
        (*this)(i) -= m(i);
}

template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix& m) const {
    if( col() != m.row() )
        throw ex::DimensionMismatch();

    Matrix<T> n({row(),m.col()});
    for(unsigned i=0;i<row();i++){
        for(unsigned j=0;j<m.col();j++){
            T sum = 0;
            for(unsigned k=0;k<col();k++)
                sum += (*this)(i,k) * m(k,j);
            n(i,j) = sum;
        }
    }
    return n;
}

template<class T>
void Matrix<T>::operator*=(const Matrix& m){
    Matrix<T> t = (*this)*m;
    *this = t;
}

// Another variant of multiplication
// because Matrix<T> multiplication is commutative
template<class T>
void Matrix<T>::operator/=(const Matrix& m) {
    Matrix<T> t = m*(*this);
    *this = t;
}

template<class T>
void Matrix<T>::operator*=(T f){
    for(int i=0;i<space();i++)
        (*this)(i) *= f;
}

template<class T>
Matrix<T> Matrix<T>::operator*(T f) const {
    Matrix<T> n(*this);
    n*=f;
    return n;
}

template<class T>
void Matrix<T>::operator/=(T f){
    (*this)*=1/f;
}

template<class T>
Matrix<T> Matrix<T>::operator/(T f) const {
    Matrix<T> n(*this);
    n/=f;
    return n;
}

template<class T>
void Matrix<T>::print() const {
    std::cout.precision(2);
    for(int i=0;i<row();i++){
        for(int j=0;j<col();j++){
            std::cout <<  std::fixed << std::setw(8) << std::right << (*this)(i,j);
        }
        std::cout << "\n";
    }
    std::cout << std::setw(8*col()+1) << std::right << row() << "x" << col() << "\n";
}

#endif
