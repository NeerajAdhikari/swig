#include "Shader.h"
#include "TfMatrix.h"

Shader::Shader(Drawer* drawer) : mp_drawer(drawer) {
}

Shader::~Shader() {
}

/* Draw a frame on the screen */
void Shader::draw() {

    // Initialize some good colors
    Color black = {0,0,0,255};
    Color white = {255,255,255,255};
    Color badcolor = {255,0,255,255};

    // Apply camera projection and perspective
    // projection transformation
    // Change homogeneous co-ordinate system
    // to device co-ordinate system
    Matrix<float>transformation =
        TfMatrix::toDevice(mp_drawer->getWidth(),
                mp_drawer->getHeight(), ScreenPoint::maxDepth)
        *TfMatrix::perspective(95,(float)mp_drawer->getWidth()
                /mp_drawer->getHeight(),10000,5)
        *TfMatrix::lookAt(m_camera.vrp,m_camera.vpn,m_camera.vup);
    bool BACKFACEDETECTION, UNBOUNDED, GOURAUD;

    for (int k=0; k<m_objects.size(); k++) {
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
            m_objects[k]->initColors(m_objects[k]->vertexCount());
            for(auto i=0;i<m_objects[k]->vertexCount();i++) {

                // An object may have surfaces of
                // different materials
                const Material& material =
                    m_objects[k]->material();
                // Normal for lighting calculation
                Vector normal = m_objects[k]->getVertexNormal(i);
                // Position for lighting calculation
                Vector position = m_objects[k]->getVertex(i);

                // Inverting the back surfaces for
                // unbounded objects
                if (UNBOUNDED && Vector::cosine((
                                position-m_camera.vrp),normal)>0)
                    normal *=-1;

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
                        intensity}).intensityAt(m_camera.vrp);
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
                        intensity}).intensityAt(m_camera.vrp);

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
                    m_objects[k]->getColor(GOURAUD?index:i));

            index = m_objects[k]->getSurface(i).y;
            ScreenPoint b(m_objects[k]->getCopyVertex(index),
                    m_objects[k]->getColor(GOURAUD?index:i));

            index = m_objects[k]->getSurface(i).z;
            ScreenPoint c(m_objects[k]->getCopyVertex(index),
                    m_objects[k]->getColor(GOURAUD?index:i));

            // overwrite is enabled for
            // non backface surfaces
            mp_drawer->fillD(a,b,c,GOURAUD, m_objects[k]->
                    getSurface(i).visible);
        }
    }

    // Update framebuffer
    mp_drawer->update();
}

