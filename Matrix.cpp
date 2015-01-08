#include "Matrix.h"

Matrix Matrix::transformation = Matrix(Pair<unsigned>({4,4}));

const Matrix& Matrix::translation(const Triplet<float>& t){
    transformation.initialize(
            1,0,0,t.x,
            0,1,0,t.y,
            0,0,1,t.z,
            0,0,0,1
            );
    return transformation;
}

const Matrix& Matrix::scaling(const Triplet<float>& s, const Triplet<float>& o){
    // NOTE: Matrix is  static because it has fixed space
    // and scaling matrix can be required many times
    transformation.initialize(
            s.x,0,0,(1-s.x)*o.x,
            0,s.y,0,(1-s.y)*o.y,
            0,0,s.z,(1-s.z)*o.z,
            1,1,1,1
            );
    return transformation;
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

    for(int i=0;i<space();i++)
        (*this)(i) = m(i);
}

void Matrix::operator=(const Matrix& m){
    // Assigning itself to itself
    if( this == &m )
        return;

    // If the space is equivalent
    // then things can be easily overwritten
    if(space() != m.space()){
        delete []m_matrix;
        m_matrix = new float[m.space()];
    }

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();
    for(int i=0;i<space();i++)
        (*this)(i) = m(i);
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
void Matrix::operator%=(const Matrix& m) {
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

void Matrix::print(){
    std::cout.precision(2);
    for(int i=0;i<row();i++){
        for(int j=0;j<col();j++){
            std::cout <<  std::fixed << std::setw(8) << std::right << (*this)(i,j);
        }
        std::cout << "\n";
    }
    std::cout << std::setw(8*col()+1) << std::right << row() << "x" << col() << "\n";
}

