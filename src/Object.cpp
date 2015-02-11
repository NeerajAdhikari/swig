#include "Object.h"

Object::Object (unsigned vertex_count):
    m_vertex({4,vertex_count}),
    m_vertex_normal({4,vertex_count}),
    m_cpyvertex({4,vertex_count})
{
    // Initialize the points
    for(int i=0;i < vertexCount();i++)
        m_vertex(3,i) = 1;
    // Initialize edges and surfaces
}

// Load an object from an .obj file
Object::Object(const std::string& filename) :
    m_vertex({4,1}),
    m_vertex_normal({4,1}),
    m_cpyvertex({4,1})
{
    // throw exception if bad bit or fail bit
    std::ifstream objfile(filename,std::ios::in);
    // TODO if enabled doesn't work for "gourd.obj"
    // objfile.exceptions( std::ifstream::failbit | std::ifstream::badbit );

    std::string line,keyword;

    // Precalculate the vertex_count
    // Preestimate the face_count
    unsigned vcount = 0;
    unsigned fcount = 0;
    while(!objfile.eof()){
        std::getline(objfile,line);
        rtrim(line);
        if(line.size()<3)
            continue;
        std::istringstream linestrm(line);
        linestrm>>keyword;
        if(keyword=="v"){
            vcount++;
        } else if(keyword=="f"){
            fcount++;
        }
    }

    m_vertex.readjust({4,vcount});
    resetCopy();
    m_vertex_normal.readjust({4,vcount});
    m_surface.reserve(fcount);

    // Read the file again
    objfile.clear();
    objfile.seekg(0 , std::ios::beg);

    unsigned vertex_count = 0;
    while (!objfile.eof()) {
        std::getline(objfile,line);
        rtrim(line);
        if (line.size()<3)
            continue;
        std::istringstream linestrm(line);

        linestrm>>keyword;
        if (keyword=="v") {
            Vector vtxpoint(0,0,0,1);
            linestrm >> vtxpoint.x >> vtxpoint.y >> vtxpoint.z;
            setVertex(vertex_count,vtxpoint);
            vertex_count++;
        } else if (keyword=="f") {
            std::string facepoint;
            std::vector<unsigned> face;
            size_t space=line.find(' ');
            line=line.substr(space+1,line.size()-space-1);
            while (space!=std::string::npos) {
                space=line.find(' ');
                if (space!=std::string::npos) {
                    facepoint=line.substr(0,space);
                    line=line.substr(space+1,line.size()-space-1);
                } else {
                    facepoint=line; line="";
                }
                size_t slash=facepoint.find('/');
                if (slash!=std::string::npos) {
                    facepoint=facepoint.substr(0,slash);
                }
                face.push_back(std::stoi(facepoint)-1);
            }
            std::vector<Triplet<unsigned> > tlst = tesselate(face);
            for (auto it=tlst.begin();it<tlst.end(); it++) {
                setSurface(*it);
            }
        }
    }
    // TODO load vertex normals for the file
    // Initialize the normal to the surfaces
    initNormal();
}

// Tesselate a polygon to triangles
std::vector<Triplet<unsigned> > Object::tesselate(
        std::vector<unsigned> face) {
    if (face.size()<3) {
        throw ex::InitFailure();
    } else {
        unsigned v1,v2,v3;
        std::vector<Triplet<unsigned> > tesselated;
        while (face.size()) {
            v1 = face.back(); face.pop_back();
            v2 = face.back(); face.pop_back();
            v3 = face.back(); face.pop_back();
            tesselated.push_back({v3,v2,v1});
            if (face.size()) {
                face.push_back(v3); face.push_back(v1);
            }
        }
        return tesselated;
    }
}

void Object::showVx() const {
    for (auto i=0; i<vertexCount(); i++) {
        getVertex(i).display();
    }
}

// TODO calculating vertex normal by averaging
// Bad bad idea
void Object::initNormal() {

    // Get normals
    m_vertex_normal.clear();
    for(auto i=0;i<m_surface.size();i++) {
        Vector  normal = getSurfaceNormal(i);
        Surface surf = getSurface(i);
        m_vertex_normal(0,surf.x) += normal.x;
        m_vertex_normal(1,surf.x) += normal.y;
        m_vertex_normal(2,surf.x) += normal.z;
        m_vertex_normal(3,surf.x) += normal.w;

        m_vertex_normal(0,surf.y) += normal.x;
        m_vertex_normal(1,surf.y) += normal.y;
        m_vertex_normal(2,surf.y) += normal.z;
        m_vertex_normal(3,surf.y) += normal.w;

        m_vertex_normal(0,surf.z) += normal.x;
        m_vertex_normal(1,surf.z) += normal.y;
        m_vertex_normal(2,surf.z) += normal.z;
        m_vertex_normal(3,surf.z) += normal.w;
    }

    // Normalize
    for(auto i=0;i<m_vertex.col();i++){
        float x =m_vertex_normal(0,i);
        float y =m_vertex_normal(1,i);
        float z =m_vertex_normal(2,i);
        float mag = std::pow(x*x+y*y+z*z,0.5);
        m_vertex_normal(0,i) /= mag;
        m_vertex_normal(1,i) /= mag;
        m_vertex_normal(2,i) /= mag;
    }

}
