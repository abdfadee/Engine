#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../shader.h"


using namespace glm;


/* Shaders */
class Shaders {
public:
    inline static Shader
        *Geometry,
        *PointLight,
        *RectAreaLight,
        *SpotLight,
        *AmbientLight;



    static void compile() {
        Geometry = new Shader("lib/renderer/core/shading/shaders/Vertex/Geometry.vert", "lib/renderer/core/shading/shaders/Fragment/Geometry.frag");
        PointLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/PointLight.frag");
        RectAreaLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/RectAreaLight.frag");
        SpotLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/SpotLight.frag");
        AmbientLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/AmbientLight.frag");
    }


    static void bindBuffers() {
        Geometry->bindBlock("CameraBuffer", 0);
        PointLight->bindBlock("CameraBuffer", 0);
    }


    static void prepare() {
        Shaders::compile();
        Shaders::bindBuffers();
    }


};