#ifndef __SURFACE__
#define __SURFACE__

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ScreenPoint.h"
#include "Material.h"
#include "mathematics/Matrix.h"
#include "mathematics/Vector.h"

// A edge contains 2 index points
struct Edge :public Pair<unsigned> {

    // Constructor
    Edge(unsigned xx, unsigned yy)
        : Pair<unsigned>(xx,yy) {}
    // Some other attributes;
    // like line color, width
};

// A surface contains 3 index points
struct Surface : public Triplet<unsigned> {

    // A normal vector
    Vector normal;

    // Constructor
    Surface(unsigned xx, unsigned yy, unsigned zz)
        : Triplet<unsigned>(xx,yy,zz)
    {}

    // like color, luminosity, texture

};

// Work on progress
// An Object is collection of Vertices, Edges and Surfaces
class Object{

    private:
        // A matrix is (4 x count) matrix is used to represent m_vertex
        // A homogeneous vertex matrix
        Matrix<float> m_vertex;
        Matrix<float> m_cpyvertex;

        // TODO added here
        Matrix<float> m_vertex_normal;

        // A vector of pair of indexes to represent edges
        std::vector<Edge> m_edge;
        // A vector of triplet of indexes to represent surfaces (triangles)
        std::vector<Surface> m_surface;

    public:

        // Defines the color and surface properties
        Material material;

        Object (unsigned vertex_count);

        // Load an object from an .obj file
        Object(const std::string& filename);

        void initNormal();

        // Tesselate a polygon to triangles
        std::vector<Triplet<unsigned> > tesselate(std::vector<unsigned> face);

        // get total no. of vertices in the matrix
        unsigned vertexCount() const ;

        // get total no. of edge in the matrix
        unsigned edgeCount() const ;

        // get total no. of surface in the matrix
        unsigned surfaceCount() const ;

        // Retuns the vertex matrix
        Matrix<float>& vmatrix() ;

        // Retuns the copy of vertex matrix
        Matrix<float>& vcmatrix() ;

        // Reset and initialize the value of copy
        // matrix to vertex matrix
        // TODO should be private
        void resetCopy();

        // Retuns the vertex matrix
        Matrix<float>& nmatrix() ;

        // Getter in form matrix(x,y)
        float& operator()(unsigned row, unsigned col);

        // Setter in form matrix(x,y)
        const float& operator()(unsigned row, unsigned col) const ;

        // Getter in the form matrix(p)
        const float& operator()(unsigned place) const ;

        // Setter in the form matrix(p)
        float& operator()(unsigned place);

        // setVertex overwrites, others append
        void setVertex(unsigned point,const Vector& p);

        void setEdge(const Pair<unsigned>& p) ;

        void setSurface(const Triplet<unsigned>& p) ;

        Vector getVertex(unsigned point) const ;

        Vector getCopyVertex(unsigned point) const ;

        Edge getEdge(unsigned point) ;

        Surface getSurface(unsigned point) ;

        // TODO for now vertex count and vertex normal count is equal
        // and vertexNormal isn't calculated when required
        // it is already saved in a matrix unlike surface normal
        Vector getVertexNormal(unsigned i) const ;

        Vector getSurfaceNormal(const Surface& p);

        Vector getSurfaceNormal(unsigned point) ;

        Vector getSurfaceCentroid(unsigned point) ;

        void showVx() const;
};


inline unsigned Object::vertexCount() const {
    return m_vertex.col();
}

inline unsigned Object::edgeCount() const {
    return m_edge.size();
}

inline unsigned Object::surfaceCount() const {
    return m_surface.size();
}

inline Matrix<float>& Object::vmatrix() {
    return m_vertex;
}

// Don't call this function time and again
// instead use a reference to store it
inline Matrix<float>& Object::vcmatrix() {
    resetCopy();
    return m_cpyvertex;
}

inline void Object::resetCopy() {
    m_cpyvertex = m_vertex;
}

inline Matrix<float>& Object::nmatrix() {
    return m_vertex_normal;
}

inline float& Object::operator()(unsigned row, unsigned col){
    return m_vertex(row,col);
}

inline const float& Object::operator()(unsigned row, unsigned col) const {
    return m_vertex(row,col);
}

inline const float& Object::operator()(unsigned place) const {
    return m_vertex(place);
}

inline float& Object::operator()(unsigned place){
    return m_vertex(place);
}

void inline Object::setVertex(unsigned point,const Vector& p){
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    m_vertex(0,point) = p.x;
    m_vertex(1,point) = p.y;
    m_vertex(2,point) = p.z;
    m_vertex(3,point) = p.w;
}

inline void Object::setEdge(const Pair<unsigned>& p) {
    if(p.x >= vertexCount() || p.y >= vertexCount())
        throw ex::OutOfBounds();

    m_edge.push_back({p.x,p.y});
}

inline void Object::setSurface(const Triplet<unsigned>& p) {
    if(p.x>=vertexCount()||p.y>=vertexCount()||p.z>=vertexCount())
        throw ex::OutOfBounds();

    Surface surf(p.x,p.y,p.z);
    //surf.normal = getSurfaceNormal(surf);
    m_surface.push_back(surf);
}

inline Vector Object::getVertex(unsigned point) const {
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_vertex(0,point),m_vertex(1,point),m_vertex(2,point),m_vertex(3,point));
}

inline Vector Object::getCopyVertex(unsigned point) const {
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_cpyvertex(0,point),m_cpyvertex(1,point),m_cpyvertex(2,point),m_cpyvertex(3,point));
}

inline Edge Object::getEdge(unsigned point) {
    if(point>=edgeCount())
        throw ex::OutOfBounds();
    return m_edge[point];
}

inline Surface Object::getSurface(unsigned point) {
    if(point>=surfaceCount())
        throw ex::OutOfBounds();
    return m_surface[point];
}

inline Vector Object::getVertexNormal(unsigned i) const {
    if(i >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_vertex_normal(0,i),m_vertex_normal(1,i),m_vertex_normal(2,i),m_vertex_normal(3,i));
}

inline Vector Object::getSurfaceNormal(const Surface& p){
    Vector v1=getVertex(p.x);
    Vector v2=getVertex(p.y);
    Vector v3=getVertex(p.z);

    Vector sidea=v2-v1;
    Vector sideb=v3-v2;

    return (sidea*sideb).normalized();
}

inline Vector Object::getSurfaceNormal(unsigned point) {
    return getSurfaceNormal(getSurface(point));
}

inline Vector Object::getSurfaceCentroid(unsigned point) {
    Surface p = getSurface(point);
    Vector v1=getVertex(p.x);
    Vector v2=getVertex(p.y);
    Vector v3=getVertex(p.z);
    return Vector((v1.x+v2.x+v3.x)/3,(v1.y+v2.y+v3.y)/3,(v1.z+v2.z+v3.z)/3,1);
}

#endif
