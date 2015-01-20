#include "MatrixInt.h"

MatrixInt::~MatrixInt(){
    delete []m_matrix;
}

MatrixInt::MatrixInt(const Pair<unsigned>& size) :
    m_row(size.x), m_col(size.y), m_space(size.x*size.y)
{
    m_matrix  = new uint32_t[space()];
}

MatrixInt::MatrixInt(const MatrixInt& m){

    m_matrix = new uint32_t[m.space()];

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();

    for(int i=0;i<space();i++)
        (*this)(i) = m(i);
}

void MatrixInt::addRow() {
    m_row++;
    uint32_t* newmat = new uint32_t[m_row*m_col];
    memcpy(newmat,m_matrix,sizeof(uint32_t)*space());
    m_space = m_row*m_col;
    memset(newmat+(m_row-1)*m_col,0,m_col);
    delete[] m_matrix;
    m_matrix = newmat;
}

void MatrixInt::addColumn() {
    uint32_t* newmat = new uint32_t[m_row*(m_col+1)];
    for (auto i=0; i<m_row; i++) {
        memcpy(newmat+i*(m_col+1),m_matrix+i*m_col,
                m_col*sizeof(uint32_t));
        newmat[(i+1)*(m_col+1)-1] = 0;
    }
    m_col++; m_space=m_col*m_row;
    delete[] m_matrix;
    m_matrix = newmat;
}

void MatrixInt::operator=(const MatrixInt& m){
    // Assigning itself to itself
    if( this == &m )
        return;

    // If the space is equivalent
    // then things can be easily overwritten
    if(space() != m.space()){
        delete []m_matrix;
        m_matrix = new uint32_t[m.space()];
    }

    m_row=m.row();
    m_col=m.col();
    m_space=m.space();
    for(int i=0;i<space();i++)
        (*this)(i) = m(i);
}

void MatrixInt::print() const {
    std::cout.precision(2);
    for(int i=0;i<row();i++){
        for(int j=0;j<col();j++){
            std::cout <<  std::fixed << std::setw(8) << std::right << (*this)(i,j);
        }
        std::cout << "\n";
    }
    std::cout << std::setw(8*col()+1) << std::right << row() << "x" << col() << "\n";
}

void MatrixInt::clear() {
    memset(m_matrix,0,space()*sizeof(uint32_t));
}
