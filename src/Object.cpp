#include "Object.h"

Object::Object (unsigned vertex_count, const Material& m,
        Shading sh, bool backface, bool bothside):
    m_vertex({4,vertex_count}),
    m_vertex_normal({4,vertex_count}),
    m_copy_vertex({4,vertex_count}),
    m_material(m),
    m_colors(NULL),
    m_colors_count(0),
    m_shading(sh),
    m_backface(backface),
    m_bothsides(bothside)
{
    // Initialize the points
    for(int i=0;i < vertexCount();i++)
        m_vertex(3,i) = 1;
    // Initialize edges and surfaces

    // If both sides need to be shaded, turn of backface detection
    if (m_bothsides)
        m_backface = true;
}

// Load an object from an .obj file
Object::Object(const std::string& filename,const Material& m,
        Shading sh, bool backface, bool bothside) :
    m_vertex({4,1}),
    m_vertex_normal({4,1}),
    m_copy_vertex({4,1}),
    m_material(m),
    m_colors(NULL),
    m_shading(sh),
    m_backface(backface),
    m_bothsides(bothside)
{
    // Turn off backface detection if both sides need to be shaded
    if (m_bothsides)
        m_backface = true;

    // throw exception if bad bit or fail bit
    std::ifstream objfile(filename,std::ios::in);
    // TODO if enabled doesn't work for "gourd.obj"
    // objfile.exceptions( std::ifstream::failbit | std::ifstream::badbit );

    std::string line,keyword;

    // Precalculate the vertex_count
    // Preestimate the face_count
    unsigned vcount = 0;
    unsigned fcount = 0;
    unsigned vncount = 0;

    while(!objfile.eof()){
        std::getline(objfile,line);
        rtrim(line);
        if(line.size()<3)
            continue;
        std::istringstream linestrm(line);
        linestrm>>keyword;
        if (keyword=="v")
            vcount++;
        else if (keyword=="f")
            fcount++;
        else if (keyword=="vn")
            vncount++;
    }

    // Loading surfaces
    m_surface.reserve(fcount);
    // Loading vertex normals
    m_vertex_normal.readjust({4,vncount});
    // Loading vertex
    m_vertex.readjust({4,vcount});
    resetCopy();

    unsigned vertex_count = 0;
    unsigned vertex_normal_count = 0;

    // Read the file again
    objfile.clear();
    objfile.seekg(0 , std::ios::beg);
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
        }
        else if (keyword=="vn") {
            Vector vtxpoint(0,0,0,0);
            linestrm >> vtxpoint.x >> vtxpoint.y >> vtxpoint.z;
            setVertexNormal(vertex_normal_count,vtxpoint);
            vertex_normal_count++;
        }

        else if (keyword=="f") {

            std::string facepoint;
            std::vector<unsigned> faceV;
            std::vector<unsigned> faceVt;
            std::vector<unsigned> faceVn;
            std::vector<std::string> indices;
            bool noNormal = false;

            // Trimming from the left
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
                indices = split(facepoint,'/');
                if (indices.size()>3)
                    throw ex::BadFileFormat();
                faceV.push_back(std::stoi(indices[0])-1);
                if (indices.size()>2 && indices[2].size()!=0)
                    faceVn.push_back(std::stoi(indices[2])-1);
                else {
                    noNormal = true;
                }

                /*size_t slash=facepoint.find('/');
                if (slash!=std::string::npos) {
                    facepoint=facepoint.substr(0,slash);
                }
                faceV.push_back(std::stoi(facepoint)-1);*/
            }

            // Returns a list of Triplet-Pairs, of which the first
            // is the vertex index triplet and the second is the
            // vertext normal index triplet.
            std::vector<Pair<Triplet<unsigned> > > tlst;
            if (noNormal)
                tlst = tesselate(faceV);
            else
                tlst = tesselate(faceV,faceVn);
            for (auto it=tlst.begin();it<tlst.end(); it++) {
                Surface* surf;
                if (noNormal)
                    surf = new Surface(it->x.x,it->x.y,it->x.z);
                else
                    surf = new Surface(it->x.x,it->x.y,it->x.z,
                        it->y.x,it->y.y,it->y.z);
                setSurface(*surf);
            }
        }
    }
    // TODO load vertex normals for the file
    // Initialize the normal to the surfaces
    //initNormal();
}

// Tesselate a polygon to triangles
std::vector<Pair<Triplet<unsigned> > > Object::tesselate(
        std::vector<unsigned> faceV, std::vector<unsigned> faceVn) {

    bool normal = (faceVn!=std::vector<unsigned>());
    std::vector<Pair<Triplet<unsigned> > > tesselated;

    if (faceV.size()<3) {
        throw ex::InitFailure();
    } else if (faceV.size()==3) {
        Pair<Triplet<unsigned> >* pr;
        if (normal)
            pr = new Pair<Triplet<unsigned> >
                ({faceV[0],faceV[1],faceV[2]},
                {faceVn[0],faceVn[1],faceVn[2]});
        else
            pr = new Pair<Triplet<unsigned> >
                ({faceV[0],faceV[1],faceV[2]},{0,0,0});
        tesselated.push_back(*pr);
        return tesselated;
    } else {
        unsigned v1,v2,v3;
        unsigned vn1,vn2,vn3;
        while (faceV.size()) {
            v1 = faceV.back(); faceV.pop_back();
            v2 = faceV.back(); faceV.pop_back();
            v3 = faceV.back(); faceV.pop_back();
            if (normal) {
                vn1 = faceVn.back(); faceVn.pop_back();
                vn2 = faceVn.back(); faceVn.pop_back();
                vn3 = faceVn.back(); faceVn.pop_back();
                tesselated.push_back({{v3,v2,v1},{vn3,vn2,vn1}});
            } else
                tesselated.push_back({{v3,v2,v1},{0,0,0}});
            if (faceV.size()) {
                faceV.push_back(v3); faceV.push_back(v1);
                if (normal) {
                    faceVn.push_back(vn3); faceVn.push_back(vn1);
                }
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
    m_vertex_normal.readjust({4,vertexCount()});
    m_vertex_normal.clear();
    for(auto i=0;i<m_surface.size();i++) {
        Vector  normal = getSurfaceNormal(i);
        Surface& surf = getSurface(i);
        surf.vertexNormals = true;
        surf.nx = surf.x; surf.ny = surf.y; surf.nz = surf.z;
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
