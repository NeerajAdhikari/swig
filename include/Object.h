#ifndef __SURFACE__
#define __SURFACE__

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ScreenPoint.h"
#include "PointLight.h"
#include "Material.h"
#include "mathematics/Matrix.h"
#include "mathematics/Vector.h"

enum class Shading { flat, gouraud, phong };

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

    // For backface detection
    bool visible;

    // Constructor
    Surface(unsigned xx, unsigned yy, unsigned zz)
        : Triplet<unsigned>(xx,yy,zz), visible(true)
    {}

    // like color, luminosity, texture
};

// Work on progress
// An Object is collection of Vertices, Edges and Surfaces
class Object {

    private:
        // A matrix is (4 x count) matrix is used to represent m_vertex
        // A homogeneous vertex matrix
        Matrix<float> m_vertex;
        // TODO we are requiring two copy vertex for shadow
        // A copy of the homogeneous matrix for manipulation
        Matrix<float> m_copy_vertex;
        // TODO no idea what to do here
        Matrix<float> m_vertex_normal;

        // A vector of pair of indexes to represent edges
        std::vector<Edge> m_edge;
        // A vector of triplet of indexes to represent
        // surfaces (triangles)
        std::vector<Surface> m_surface;

        // Does a certain light source illuminate a surface?
        Matrix<int> m_bright;

        // Point Light sources
        std::vector<PointLight*> m_lights;

        // Defines the color and surface properties
        Material m_material;

        // The type of shading
        Shading m_shading;

        // Whether to do backface culling on the object
        bool m_backface;

        // Whether the object's surfaces need to be shaded both sides
        bool m_bothsides;

        // Store color information for lighting
        Color* m_colors;

        int m_colors_count;

        // Reset and initialize the value of copy_vertex
        void resetCopy();

        // TODO load normals not calculate
        // void initNormal();

        // Tesselate a polygon to triangles
        std::vector<Triplet<unsigned> > tesselate(std::vector<unsigned> face);

    public:

        // Load an object from an .obj file
        Object(const std::string& filename, const Material& m, Shading sh,
                bool backface = true, bool bothside = false);
        Object (unsigned vertex_count, const Material& m, Shading sh,
                bool backface = true, bool bothside = false);
        ~Object();

        void initColors(unsigned size){
            if(size!=m_colors_count){
                if( m_colors != NULL)
                    delete []m_colors;
                m_colors = new Color[size];
                m_colors_count = size;
            }
        }

        void deleteColors(){
            if( m_colors != NULL)
                delete []m_colors;
            m_colors = NULL;
            m_colors_count = 0;
        }

        Color& getColor(unsigned size){
            if(size >= m_colors_count)
                throw ex::OutOfBounds();
            return m_colors[size];
        }

        // Return the material of the object
        const Material& material() const;

        // Retuns matrix
        Matrix<float>& vmatrix() ;
        Matrix<float>& vcmatrix() ;
        Matrix<float>& vnmatrix() ;

        // Get total counts
        unsigned vertexCount() const ;
        unsigned edgeCount() const ;
        unsigned surfaceCount() const ;

        // setVertex overwrites, others append
        void setVertex(unsigned point,const Vector& p);
        void setEdge(const Pair<unsigned>& p) ;
        void setSurface(const Triplet<unsigned>& p) ;

        // Get Edge
        Edge& getEdge(unsigned point) ;

        // Get Surface
        Surface& getSurface(unsigned point) ;

        // Get Vertex
        Vector getVertex(unsigned point) const ;
        Vector getCopyVertex(unsigned point) const ;
        Vector getDistortedVertex(unsigned point) const ;

        // Get Normal
        Vector getVertexNormal(unsigned i) const ;
        Vector getSurfaceNormal(unsigned point) ;
        Vector getDistortedSurfaceNormal(unsigned point);

        // Get Centroid
        Vector getSurfaceCentroid(unsigned point) ;

        void showVx() const;

        Shading getShading() const;
        void setShading(Shading sh);
        bool backface() const;
        void backface(bool bf);
        bool bothsides() const;
        void bothsides(bool bs);

        // TODO load normals not calculate
        void initNormal();

        void initBright(std::vector<PointLight*> lights);

        bool onShadow(const Vector& point, int lyt);
        void showBright();
};

inline Object::~Object(){
    if( m_colors != NULL)
        delete []m_colors;
}

inline const Material& Object::material() const {
    return m_material;
}

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

inline Matrix<float>& Object::vnmatrix() {
    return m_vertex_normal;
}

inline Matrix<float>& Object::vcmatrix() {
    // Don't call this function time and again
    // instead use a reference to store it
    resetCopy();
    return m_copy_vertex;
}

inline void Object::resetCopy() {
    m_copy_vertex = m_vertex;
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
    m_surface.push_back(surf);
}

inline Vector Object::getVertex(unsigned point) const {
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_vertex(0,point),
            m_vertex(1,point),
            m_vertex(2,point),
            m_vertex(3,point));
}

inline Vector Object::getCopyVertex(unsigned point) const {
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_copy_vertex(0,point),
            m_copy_vertex(1,point),
            m_copy_vertex(2,point),
            m_copy_vertex(3,point));
}

inline Vector Object::getDistortedVertex(unsigned point) const {
    if(point >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_copy_vertex(0,point),
            m_copy_vertex(1,point),
            m_copy_vertex(2,point),
            m_vertex(3,point));
}

inline Edge& Object::getEdge(unsigned point) {
    if(point>=edgeCount())
        throw ex::OutOfBounds();
    return m_edge[point];
}

inline Surface& Object::getSurface(unsigned point) {
    if(point>=surfaceCount())
        throw ex::OutOfBounds();
    return m_surface[point];
}

inline Vector Object::getVertexNormal(unsigned i) const {
    if(i >= vertexCount())
        throw ex::OutOfBounds();
    return Vector(m_vertex_normal(0,i),
            m_vertex_normal(1,i),
            m_vertex_normal(2,i),
            m_vertex_normal(3,i));
}

inline Vector Object::getSurfaceNormal(unsigned point) {
    Surface& p = getSurface(point);
    Vector v1=getVertex(p.x);
    Vector v2=getVertex(p.y);
    Vector v3=getVertex(p.z);

    Vector sidea= v2-v1;
    Vector sideb= v3-v2;
    return (sidea*sideb).normalized();
}

inline Vector Object::getDistortedSurfaceNormal(unsigned point){
    Surface& p = getSurface(point);
    Vector v1=getDistortedVertex(p.x);
    Vector v2=getDistortedVertex(p.y);
    Vector v3=getDistortedVertex(p.z);

    Vector sidea = v2-v1;
    Vector sideb = v3-v2;

    return (sidea*sideb).normalized();
}

inline Vector Object::getSurfaceCentroid(unsigned point) {
    Surface p = getSurface(point);
    Vector v1=getVertex(p.x);
    Vector v2=getVertex(p.y);
    Vector v3=getVertex(p.z);
    return (v1+v2+v3)/3;
    //return Vector((v1.x+v2.x+v3.x)/3,(v1.y+v2.y+v3.y)/3,(v1.z+v2.z+v3.z)/3,1);
}

inline Shading Object::getShading() const {
    return m_shading;
}

inline void Object::setShading(Shading sh) {
    m_shading = sh;
}

inline bool Object::backface() const {
    return m_backface;
}

inline void Object::backface(bool bf) {
    m_backface = bf;
}

inline bool Object::bothsides() const {
    return m_bothsides;
}

inline void Object::bothsides(bool bs) {
    m_bothsides = bs;
}

#endif
