#ifndef __SURFACE__
#define __SURFACE__

#include <vector>
#include "Matrix.h"
#include "VectorTriplet.h"

// Work on progress
// An Object is collection of Vertices, Edges and Surfaces
class Object{

    private:
    // A matrix is (4 x count) matrix is used to represent m_vertex
    // A homogeneous vertex matrix
    Matrix m_vertex;
    // A vector of pair of indexes to represent edges
    std::vector<Edge> m_edge;
    // A vector of triplet of indexes to represent surfaces (triangles)
    std::vector<Surface> m_surface;
    // The total no. of points
    unsigned m_vertex_count;

    public:
    Object (unsigned vertex_count)
        : m_vertex_count(vertex_count), m_vertex({4,vertex_count})
    {
        // Initialize the points
        for(int i=0;i < vertexCount();i++)
            m_vertex(3,i) = 1;

        // Initialize edges and surfaces
    }

    // get total no. of vertices in the matrix
    inline unsigned vertexCount() const {
        return m_vertex_count;
    }

    // get total no. of edge in the matrix
    inline unsigned edgeCount() const {
        return m_edge.size();
    }

    // get total no. of surface in the matrix
    inline unsigned surfaceCount() const {
        return m_surface.size();
    }

    inline Matrix& vertex() {
        return m_vertex;
    }

    // Assign as if it were a matrix, useful in transformations
    Object& operator=(const Matrix& m) {
        m_vertex = m;
        return *this;
    }

    // Matrix getter
    operator Matrix&() {
        return m_vertex;
    }

    // Matrix getter
    operator const Matrix&() const {
        return m_vertex;
    }

    // setVertex overwrites, others append
    void inline setVertex(unsigned point,const Vertex& p){
        if(point >= vertexCount())
            throw ex::OutOfBounds();
        m_vertex(0,point) = p.x;
        m_vertex(1,point) = p.y;
        m_vertex(2,point) = p.z;
        m_vertex(3,point) = 1;
    }

    inline Vertex getVertex(unsigned point) const {
        if(point >= vertexCount())
            throw ex::OutOfBounds();
        float hm = m_vertex(3,point);
        hm=(hm==0)?1:hm;
        return {m_vertex(0,point)/hm,m_vertex(1,point)/hm,m_vertex(2,point)/hm};
    }

    inline void setEdge(const Pair<unsigned>& p) {
        if(p.x >= vertexCount() || p.y >= vertexCount())
            throw ex::OutOfBounds();
        m_edge.push_back({p.x,p.y});
    }

    inline void setSurface(const Triplet<unsigned>& p) {
        if(p.x>=vertexCount()||p.y>=vertexCount()||p.z>=vertexCount())
            throw ex::OutOfBounds();
        Surface surf = {p.x,p.y,p.z};

        Vertex v1=getVertex(p.x),v2=getVertex(p.y),v3=getVertex(p.z);
        VectorTriplet sidea=v2-v1, sideb=v3-v2;
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

    inline VectorTriplet getSurfaceNormal(unsigned point) {
        if(point>=edgeCount())
            throw ex::OutOfBounds();
        Surface p = getSurface(point);
        Vertex v1=getVertex(p.x),v2=getVertex(p.y),v3=getVertex(p.z);
        VectorTriplet sidea=v2-v1, sideb=v3-v2;
       m_surface[m_surface.size()-1].normal=(sidea*sideb).normalized();
        return m_surface[m_surface.size()-1].normal;
    }

    void showVx() const {
        for (auto i=0; i<m_vertex_count; i++) {
            getVertex(i).display();
        }
    }
};

#endif
