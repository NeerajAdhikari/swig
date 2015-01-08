#ifndef __SURFACE__
#define __SURFACE__

#include <list>
#include "Matrix.h"

// TODO triangularization of a surface

struct Edge {
    // 2 index points
    Pair<unsigned> point;
    // Some other attributes;
};

// A surface represents a triangle
struct Surface {
    // 3 index points
    Triplet<unsigned> point;
    // Normal vector
    Triplet<float>  normal;
    // Some other attributes;
};


// NOTE
// Random Access on lists is slower and
// the insertion is almost always at the start of the program
// so we could use vector to facilitate random access

// An Object is collection of vertex, Edge and Surface
class Object{
    private:
        // A matrix is (4 x count) matrix is used to represent m_vertex
        // A homogeneous vertex matrix
        Matrix m_vertex;
        // A vector of pair of indexes to represent edges
        std::list<Edge> m_edge;
        // A vector of triplet of indexes to represent surfaces (triangles)
        std::list<Surface> m_surface;
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
        }

        inline Vertex getVertex(unsigned point) const {
            if(point >= vertexCount())
                throw ex::OutOfBounds();
            return Vertex({m_vertex(0,point),m_vertex(1,point),m_vertex(2,point)});
        }

        /*
        inline void setEdge(const Pair& p) {
            if(p.x >= vertexCount() || p.y >= vertexCount())
                throw ex::OutOfBounds();
            m_edge.push_back(p.x,p.y);
        }

        inline void setSurface(const Triplet& p) {
            if(p.x >= vertexCount() || p.y >= vertexCount() || p.z >= vertexCount())
                throw ex::OutOfBounds();
            m_surface.push_back(p.x,p.y,p.z);
        }
        */

        // getEdge and getSurface not implemented
};

#endif
