#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../shader.h"


using namespace glm;


/* Shaders */
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
        *ShadowMapping,
        *DirectionalDepthMap,
        *VisualizingDepth,
        *OmnidirectionalDepthMap;


    static void compile() {
        Basic = new Shader("lib/renderer/core/shading/shaders/Vertex/Basic.vert", "lib/renderer/core/shading/shaders/Fragment/Basic.frag");
        Geometry = new Shader("lib/renderer/core/shading/shaders/Vertex/Geometry.vert", "lib/renderer/core/shading/shaders/Fragment/Geometry.frag");
        PointLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/PointLight.frag");
        RectAreaLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/RectAreaLight.frag");
        SpotLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/SpotLight.frag");
        AmbientLight = new Shader("lib/renderer/core/shading/shaders/Vertex/Light.vert", "lib/renderer/core/shading/shaders/Fragment/AmbientLight.frag");
        Skybox = new Shader("Shaders/Vertex/Skybox.vert", "Shaders/Fragment/Cubemap.frag");
        ShadowMapping = new Shader("Shaders/Vertex/ShadowMapping.vert", "Shaders/Fragment/ShadowMapping.frag");
        DirectionalDepthMap = new Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/DirectionalDepthMap.frag");
        VisualizingDepth = new Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/VisualizingDepth.frag");
        OmnidirectionalDepthMap = new Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/OmnidirectionalDepthMap.frag", "Shaders/Geometry/Omnidirectional.geom");
    }

};