#ifndef __SURFACE__
#define __SURFACE__

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <initializer_list>
#include "Matrix.h"
#include "Vector.h"

// A edge contains 2 index points
struct Edge :public Pair<unsigned> {

    // Constructor
    Edge(unsigned xx, unsigned yy)
        : Pair<unsigned>(xx,yy) {}

    Edge(std::initializer_list<unsigned> il)
        : Pair<unsigned>(*il.begin(),*(il.begin()+1))
    {}

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

    Surface(std::initializer_list<unsigned> l)
        : Triplet<unsigned>(*l.begin(),*(l.begin()+1),*(l.begin()+2))
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
    // A vector of pair of indexes to represent edges
    std::vector<Edge> m_edge;
    // A vector of triplet of indexes to represent surfaces (triangles)
    std::vector<Surface> m_surface;
    // The total no. of points

    public:
    Object (unsigned vertex_count);

    // Load an object from an .obj file
    Object(const std::string& filename);

    // Tesselate a polygon to triangles
    std::vector<Triplet<unsigned> > tesselate(std::vector<unsigned>
            face);

    // get total no. of vertices in the matrix
    inline unsigned vertexCount() const {
        return m_vertex.col();
    }

    // get total no. of edge in the matrix
    inline unsigned edgeCount() const {
        return m_edge.size();
    }

    // get total no. of surface in the matrix
    inline unsigned surfaceCount() const {
        return m_surface.size();
    }

    inline Matrix<float>& vertex() {
        return m_vertex;
    }

    // setVertex overwrites, others append
    void inline setVertex(unsigned point,const Vector& p){
        if(point >= vertexCount())
            throw ex::OutOfBounds();
        m_vertex(0,point) = p.x;
        m_vertex(1,point) = p.y;
        m_vertex(2,point) = p.z;
        m_vertex(3,point) = p.w;
    }

    inline Vector getVertex(unsigned point) const {
        if(point >= vertexCount())
            throw ex::OutOfBounds();
        return {m_vertex(0,point),m_vertex(1,point),m_vertex(2,point),m_vertex(3,point)};
    }

    inline void setEdge(const Pair<unsigned>& p) {
        if(p.x >= vertexCount() || p.y >= vertexCount())
            throw ex::OutOfBounds();
        m_edge.push_back({p.x,p.y});
    }

    inline void setSurface(const Triplet<unsigned>& p) {
        if(p.x>=vertexCount()||p.y>=vertexCount()||p.z>=vertexCount()) {
            throw ex::OutOfBounds();
        }
        Surface surf = {p.x,p.y,p.z};

        Vector v1=getVertex(p.x),v2=getVertex(p.y),v3=getVertex(p.z);

        Vector sidea=v2-v1, sideb=v3-v2;
        surf.normal=(sidea*sideb).normalized();
        m_surface.push_back(surf);
    }

    inline Surface getSurface(unsigned point) {
        if(point>=surfaceCount())
            throw ex::OutOfBounds();
        return m_surface[point];
    }

    inline Edge getEdge(unsigned point) {
        if(point>=edgeCount())
            throw ex::OutOfBounds();
        return m_edge[point];
    }

    inline Vector getSurfaceNormal(unsigned point) {
        if(point>=surfaceCount())
            throw ex::OutOfBounds();
        Surface p = getSurface(point);
        Vector v1=getVertex(p.x),v2=getVertex(p.y),v3=getVertex(p.z);
        Vector sidea=v2-v1, sideb=v3-v2;
       m_surface[point].normal=(sidea*sideb).normalized();
        return m_surface[point].normal;
    }

    inline Vector getSurfaceCentroid(unsigned point) {
        if(point>=surfaceCount())
            throw ex::OutOfBounds();
        Surface p = getSurface(point);
        Vector v1=getVertex(p.x),v2=getVertex(p.y),v3=getVertex(p.z);
        return {(v1.x+v2.x+v3.x)/3,(v1.x+v2.x+v3.x)/3,(v1.x+v2.x+v3.x)/3,(v1.x+v2.x+v3.x)/3,1};
    }

    void showVx() const;
};

#endif
