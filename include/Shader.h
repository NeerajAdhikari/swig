#ifndef __SHADER_H__
#define __SHADER_H__

#include <vector>
#include "PointLight.h"
#include "AmbientLight.h"
#include "Drawer.h"
#include "Camera.h"
#include "Object.h"

/* The class Shader is the primary component of the library.
 * It does the task of creating pixels from memory objects.
 */
class Shader {

    // The drawer object used for drawing the pixels
    Drawer* mp_drawer;

    // List of pointers to objects to be drawn
    std::vector<Object*> m_objects;
    // List of point light sources
    std::vector<PointLight*> m_pointLights;
    // Ambient light
    AmbientLight m_ambientLight;
    // The camera to be used for viewing
    Camera m_camera;

    public:


    // Constructor
    Shader(Drawer* drawer);
    // Destructor
    ~Shader();

    // Draw a frame
    void draw();

    /* Getters and setters */

    // Add an Object
    int addObject(Object* obj) {
        if (obj!=NULL)
            m_objects.push_back(obj);
        return m_objects.size()-1;
    }

    // Add a point light source
    int addLight(PointLight* lyt) {
        if (lyt!=NULL)
            m_pointLights.push_back(lyt);
        return m_pointLights.size()-1;
    }

    // Set the ambient light intensity
    void setAmbient(const AmbientLight& amb) {
        m_ambientLight = amb;
    }

    // Get the ambient light intensity
    AmbientLight getAmbient() const {
        return m_ambientLight;
    }

    // Set the viewing camera
    void setCamera(const Camera& cam) {
        m_camera = cam;
    }

    // Get the viewing camera
    Camera getCamera() const {
        return m_camera;
    }

    Drawer* getDrawerP() {
        return mp_drawer;
    }

    Object* getObjectP(int i) {
        if (i>=m_objects.size())
            throw ex::OutOfBounds();
        return m_objects[i];
    }

    int objectCount() const {
        return m_objects.size();
    }
};

#endif  // __SHADER_H__
