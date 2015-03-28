#include "Shader.h"
#include "TfMatrix.h"

Shader::Shader(Drawer* drawer) : mp_drawer(drawer) {
}

Shader::~Shader() {
}

/* Draw a frame on the screen */
void Shader::draw() {

    bool BACKFACEDETECTION, UNBOUNDED, GOURAUD;

    /*for (int i=0; i<200; i++) {
        for (int j=0; j<150; j++) {
            std::cout<<m_pointLights[0]->depthAt(i,j)<<"\t";
        }
        std::cout<<std::endl;
    }
    if (m_pointLights[0]->onShadow({0,0,0,1}))
        std::cout<<"On Shadow"<<std::endl;
    else
        std::cout<<"nO Shadow"<<std::endl;
    exit(0);*/


    // Apply camera projection and perspective
    // projection transformation
    // Change homogeneous co-ordinate system
    // to device co-ordinate system
    Matrix<float>transformation =
        TfMatrix::toDevice(mp_drawer->getWidth(),
                mp_drawer->getHeight(), ScreenPoint::maxDepth)
        *TfMatrix::perspective(95,mp_drawer->getAspectRatio()
                ,10000,5)
        *TfMatrix::lookAt(m_camera.vrp,m_camera.vpn,m_camera.vup);

    for (unsigned int k=0; k<m_objects.size(); k++) {
        //m_objects[k]->vmatrix() /= rotator;
        //m_objects[k]->vnmatrix() /= rotator;

        // Get the vertex copy matrix
        // NOTE: getting a vertex matrix will reset the
        // previous content of the copy vertex matrix
        Matrix<float>& copyalias = m_objects[k]->vcmatrix();
        copyalias /= transformation;

        // Perspective divide, homogenous co-ordinates
        // to normalized co-ordinate
        // NOTE: this can be done later in life
        for (unsigned i=0; i<m_objects[k]->vertexCount(); i++) {
            copyalias(0,i) /= copyalias(3,i);
            copyalias(1,i) /= copyalias(3,i);
            copyalias(2,i) /= copyalias(3,i);
            copyalias(3,i) = 1.0;
        }

        // SURFACE SHADER
        BACKFACEDETECTION = m_objects[k]->backface();
        UNBOUNDED = m_objects[k]->bothsides();
        GOURAUD = m_objects[k]->getShading()==Shading::gouraud;

        // Detect backfaces in normalized co-ordinates
        if(BACKFACEDETECTION) {
            for(int i=0;i<m_objects[k]->surfaceCount();i++) {
                // This normal is a special kind of normal,
                // it uses x and y of
                // the projected matrix so as to get
                // orthogonal projection system
                // but original Z value for better depth
                // calculation
                Vector normal = m_objects[k]->
                    getDistortedSurfaceNormal(i);

                m_objects[k]->getSurface(i).visible=(normal.z<0);
            }
        }

        if (GOURAUD) {
            // VERTEX shader
            unsigned vnn =0;
            if (!m_objects[k]->getSurface(0).vertexNormals) {
                m_objects[k]->initNormal();
                /*std::cout<<m_objects[k]->vertexNormalCount()
                    <<std::endl;
                std::cout<<m_objects[k]->vertexCount()<<std::endl;*/
                vnn = m_objects[k]->vcmatrix().col();
                //std::cout<<vnn<<std::endl;
            }
            m_objects[k]->initColors(m_objects[k]->surfaceCount()*3);
            for(auto i=0;i<m_objects[k]->surfaceCount();i++) {

                // An object may have surfaces of
                // different materials
                const Material& material =
                    m_objects[k]->material();

                // The surface to be shaded
                Surface surf = m_objects[k]->getSurface(i);
                /*if (surf.nx>=vnn || surf.ny>=vnn || surf.nz>=vnn) {
                    std::cout<<surf.nx<<std::endl;
                    std::cout<<surf.ny<<std::endl;
                    std::cout<<surf.nz<<std::endl;
                    std::cout<<"DEgeneracy"<<i<<std::endl;
                }
                exit(0);*/

                // Normals for lighting calculation
                Vector nx = m_objects[k]->getVertexNormal(surf.nx);
                Vector ny = m_objects[k]->getVertexNormal(surf.ny);
                Vector nz = m_objects[k]->getVertexNormal(surf.nz);
                Vector normals[] = {nx,ny,nz};

                // Position for lighting calculation
                Vector positions[] = {
                    m_objects[k]->getVertex(surf.x),
                    m_objects[k]->getVertex(surf.y),
                    m_objects[k]->getVertex(surf.z)};

                for (int h=0; h<3; h++) {
                    // Inverting the back surfaces for
                    // unbounded objects
                    if (UNBOUNDED && Vector::cosine((positions[h]
                                    -m_camera.vrp),normals[h])>0)
                        normals[h] *=-1;

                    // Ambient lighting
                    Coeffecient intensity = m_ambientLight.intensity
                        *material.ka;

                    // Diffused and Specular lighting
                    for(int x=0; x<m_pointLights.size(); x++)
                        intensity += m_pointLights[x]->lightingAt(
                                positions[h],normals[h],
                                material,m_camera.vrp);

                    // Automatic conversion from Coeffecient
                    // to Color
                    // The reflection surface can be seen as a
                    // light source to camera
                    m_objects[k]->getColor(i*3+h) = PointLight(
                            {positions[h],{0,0,0,0},{0,0,0,0}},
                            intensity).intensityAt(m_camera.vrp);
                }
            }
        } else {
            // SURFACE shader
            m_objects[k]->initColors(m_objects[k]->surfaceCount());
            //colors =  new Color[obj.surfaceCount()];

            for(auto i=0;i<m_objects[k]->surfaceCount();i++) {
                // An object may have surfaces of
                // different materials
                const Material& material =
                    m_objects[k]->material();
                // Normal for lighting calculation
                Vector normal =
                    m_objects[k]->getSurfaceNormal(i);
                // Position for lighting calculation
                Vector position =  m_objects[k]->
                    getSurfaceCentroid(i);

                // If backfacedetection then continue
                // if suitable
                if (!UNBOUNDED && BACKFACEDETECTION &&
                        !m_objects[k]->getSurface(i).visible)
                    continue;
                // Inverting the back surfaces for
                // unbounded objects
                else if (UNBOUNDED && !m_objects[k]->
                        getSurface(i).visible)
                    normal *= -1;

                // Ambient lighting
                Coeffecient intensity = m_ambientLight.intensity
                    *material.ka;

                // Diffused and Specular lighting
                for(int i=0; i<m_pointLights.size(); i++)
                    intensity += m_pointLights[i]->lightingAt(
                            position,normal,material,m_camera.vrp);

                // Automatic conversion from Coeffecient
                // to Color
                // The reflection surface can be seen as a
                // light source to camera
                m_objects[k]->getColor(i) = PointLight({position,
                        {0,0,0,0},{0,0,0,0}},
                        intensity).intensityAt(m_camera.vrp);
            }
        }
    }

    // Clear framebuffer, we're about to plot
    mp_drawer->clear(badcolor);

    // Fill the surfaces
    for(int k=0;k<m_objects.size(); k++){

        BACKFACEDETECTION = m_objects[k]->backface();
        UNBOUNDED = m_objects[k]->bothsides();
        GOURAUD = m_objects[k]->getShading()==Shading::gouraud;

        for (int i=0; i<m_objects[k]->surfaceCount(); i++) {

            if (!UNBOUNDED && BACKFACEDETECTION &&
                    !m_objects[k]->getSurface(i).visible)
                continue;

            int index = m_objects[k]->getSurface(i).x;
            ScreenPoint a(m_objects[k]->getCopyVertex(index),
                    m_objects[k]->getColor(GOURAUD?(i*3):i));
            a.real = m_objects[k]->getVertex(index);

            index = m_objects[k]->getSurface(i).y;
            ScreenPoint b(m_objects[k]->getCopyVertex(index),
                    m_objects[k]->getColor(GOURAUD?(i*3+1):i));
            b.real = m_objects[k]->getVertex(index);

            index = m_objects[k]->getSurface(i).z;
            ScreenPoint c(m_objects[k]->getCopyVertex(index),
                    m_objects[k]->getColor(GOURAUD?(i*3+2):i));
            c.real = m_objects[k]->getVertex(index);

            // overwrite is enabled for
            // non backface surfaces
            mp_drawer->fillD(a,b,c,GOURAUD, this, m_objects[k]->
                    getSurface(i).visible);
        }
    }

    // Update framebuffer
    mp_drawer->update();
}


