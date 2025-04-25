#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../shader.h"


using namespace glm;


class Shaders {
public:
    inline static Shader
        *Basic,
        *Geometry,
        *PointLight,
        *RectAreaLight,
        *SpotLight,
        *AmbientLight,
        *Skybox,
        *DirectionalDepthMap,
        *VisualizingDepth,
        *OmnidirectionalDepthMap;


    static void compile() {
        Basic = new Shader("lib/renderer/shading/shaders/Vertex/Basic.vert", "lib/renderer/shading/shaders/Fragment/Basic.frag");
        Geometry = new Shader("lib/renderer/shading/shaders/Vertex/Geometry.vert", "lib/renderer/shading/shaders/Fragment/Geometry.frag");
        PointLight = new Shader("lib/renderer/shading/shaders/Vertex/Light.vert", "lib/renderer/shading/shaders/Fragment/PointLight.frag");
        RectAreaLight = new Shader("lib/renderer/shading/shaders/Vertex/Light.vert", "lib/renderer/shading/shaders/Fragment/RectAreaLight.frag");
        SpotLight = new Shader("lib/renderer/shading/shaders/Vertex/Light.vert", "lib/renderer/shading/shaders/Fragment/SpotLight.frag");
        AmbientLight = new Shader("lib/renderer/shading/shaders/Vertex/Light.vert", "lib/renderer/shading/shaders/Fragment/AmbientLight.frag");
        DirectionalDepthMap = new Shader("lib/renderer/shading/shaders/Vertex/DepthMap.vert", "lib/renderer/shading/shaders/Fragment/DirectionalDepthMap.frag");
        OmnidirectionalDepthMap = new Shader("lib/renderer/shading/shaders/Vertex/DepthMap.vert", "lib/renderer/shading/shaders/Fragment/OmnidirectionalDepthMap.frag", "lib/renderer/shading/shaders/Geometry/Omnidirectional.geom");
        VisualizingDepth = new Shader("lib/renderer/shading/shaders/Vertex/PostProcessing.vert", "lib/renderer/shading/shaders/Fragment/VisualizingDepth.frag");
        Skybox = new Shader("lib/renderer/shading/shaders/Vertex/Skybox.vert", "lib/renderer/shading/shaders/Fragment/Cubemap.frag");
    }


    static void bind() {
        Geometry->bindBlock("View",0);
        PointLight->bindBlock("View", 0);
        RectAreaLight->bindBlock("View", 0);
        SpotLight->bindBlock("View", 0);
        SpotLight->bindBlock("View", 0);
    }


};