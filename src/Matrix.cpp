#include "Matrix.h"

bool Matrix::isSquare() const {
    return row() == col();
}

Matrix Matrix::transpose() const {
    Matrix t({col(),row()});
    for(int i=0;i<row();i++)
        for(int j=0;j<col();j++)
            t(j,i) = (*this)(i,j);
    return t;
}

void Matrix::readjust(const Pair<unsigned>& size) {

    unsigned tspace = size.x * size.y;
    float* tmatrix = new float[tspace];

    if (size.y == m_col && size.x == m_row){
        return;
    } else if(size.y == m_col) {
        // If column number is equal then a block copy can be done
        unsigned smallers = tspace < space() ? tspace : space();
        std::memcpy( tmatrix, m_matrix, smallers * sizeof(float));
    } else {
        // If column number varies then a linear copy must be done
        unsigned smallerx = size.x < row() ? size.x : row();
        unsigned smallery = size.y < col() ? size.y : col();
        for(int i = 0;i<smallerx;i++)
            std::memcpy( tmatrix+i*size.y , m_matrix+i*col(), smallery* sizeof(float));
    }

    delete []m_matrix;

    m_matrix = tmatrix;
    m_row = size.x;
    m_col = size.y;
    m_space = tspace;
}

float Matrix::determinant() const {
    if(row()!=col())
        throw ex::DimensionMismatch();

    Matrix arr = *this;
    unsigned size = row();
    float det = 1;

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


Matrix Matrix::identity(unsigned n) {
    Matrix inst({n,n});
    for(int i=0;i<inst.space();i++)
        inst(i) = (i%(n+1) == 0);
    return inst;
}

Matrix Matrix::zero(const Pair<unsigned>& size){
    Matrix inst({size.x,size.y});
    for(int i=0;i<inst.space();i++)
        inst(i) = 0;
    return inst;
}

Matrix::~Matrix(){
    delete []m_matrix;
}

Matrix::Matrix(const Pair<unsigned>& size) :
    m_row(size.x), m_col(size.y), m_space(size.x*size.y)
{
    m_matrix  = new float[space()];
}

Matrix::Matrix(const Matrix& m){

    m_matrix = new float[m.space()];

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();

    std::memcpy ( m_matrix, m.m_matrix , m_space*sizeof(float) );

}

void Matrix::addRow() {
    m_row++;
    float* newmat = new float[m_row*m_col];
    memcpy(newmat,m_matrix,sizeof(float)*space());
    m_space = m_row*m_col;
    memset(newmat+(m_row-1)*m_col,0,m_col);
    delete[] m_matrix;
    m_matrix = newmat;
}

void Matrix::addColumn() {
    float* newmat = new float[m_row*(m_col+1)];
    for (auto i=0; i<m_row; i++) {
        memcpy(newmat+i*(m_col+1),m_matrix+i*m_col,
                m_col*sizeof(float));
        newmat[(i+1)*(m_col+1)-1] = 0;
    }
    m_col++; m_space=m_col*m_row;
    delete[] m_matrix;
    m_matrix = newmat;
}

void Matrix::operator=(const Matrix& m){
    // Assigning itself to itself
    if( this == &m )
        return;

    // If the space is equivalent // then things can be easily overwritten
    if(space() != m.space()){
        delete []m_matrix;
        m_matrix = new float[m.space()];
    }

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();

    std::memcpy ( m_matrix, m.m_matrix , m_space*sizeof(float) );
}

Matrix Matrix::operator+(const Matrix& m) const {
    Matrix n(*this);
    n+=m;
    return n;
}

void Matrix::operator+=(const Matrix& m) {
    if( m.row() != row() || m.col() != col())
        throw ex::DimensionMismatch();
    for(int i=0;i<space();i++)
        (*this)(i) += m(i);
}

Matrix Matrix::operator-(const Matrix& m) const {
    Matrix n(*this);
    n-=m;
    return n;
}

void Matrix::operator-=(const Matrix& m) {
    if( m.row() != row() || m.col() != col())
        throw ex::DimensionMismatch();
    for(int i=0;i<space();i++)
        (*this)(i) -= m(i);
}

Matrix Matrix::operator*(const Matrix& m) const {
    if( col() != m.row() )
        throw ex::DimensionMismatch();

    Matrix n({row(),m.col()});
    for(unsigned i=0;i<row();i++){
        for(unsigned j=0;j<m.col();j++){
            float sum = 0;
            for(unsigned k=0;k<col();k++)
                sum += (*this)(i,k) * m(k,j);
            n(i,j) = sum;
        }
    }
    return n;
}

void Matrix::operator*=(const Matrix& m){
    Matrix t = (*this)*m;
    *this = t;
}

// Another variant of multiplication
// because Matrix multiplication is commutative
void Matrix::operator/=(const Matrix& m) {
    Matrix t = m*(*this);
    *this = t;
}

void Matrix::operator*=(float f){
    for(int i=0;i<space();i++)
        (*this)(i) *= f;
}

Matrix Matrix::operator*(float f) const {
    Matrix n(*this);
    n*=f;
    return n;
}

void Matrix::operator/=(float f){
    (*this)*=1/f;
}

Matrix Matrix::operator/(float f) const {
    Matrix n(*this);
    n/=f;
    return n;
}

void Matrix::print() const {
    std::cout.precision(2);
    for(int i=0;i<row();i++){
        for(int j=0;j<col();j++){
            std::cout <<  std::fixed << std::setw(8) << std::right << (*this)(i,j);
        }
        std::cout << "\n";
    }
    std::cout << std::setw(8*col()+1) << std::right << row() << "x" << col() << "\n";
}
