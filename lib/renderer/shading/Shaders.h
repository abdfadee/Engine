#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "../core/Mesh.h"
#include "../geometry/RectangleGeometry.h"
#include "../geometry/BoxGeometry.h"


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
        *PostProcessing,
        *Skybox,
        *DirectionalDepthMap,
        *VisualizingDepth,
        *OmnidirectionalDepthMap,
        *EquirectangularToCubemap,
        *IrradianceConvolution,
        *HDRPrefilter,
        *BRDF,
        *IBL,
        *GLTFGeometery;


    inline static Mesh* screen, * unitBox;



    static void compile() {
        Basic = new Shader("lib/renderer/shading/Vertex/Basic.vert", "lib/renderer/shading/Fragment/Basic.frag");

        Geometry = new Shader("lib/renderer/shading/Vertex/Geometry.vert", "lib/renderer/shading/Fragment/Geometry.frag");
        GLTFGeometery = new Shader("lib/renderer/shading/Vertex/Geometry.vert", "lib/renderer/shading/Fragment/GLTFGeometery.frag");

        PointLight = new Shader("lib/renderer/shading/Vertex/Light.vert", "lib/renderer/shading/Fragment/PointLight.frag");
        RectAreaLight = new Shader("lib/renderer/shading/Vertex/Light.vert", "lib/renderer/shading/Fragment/RectAreaLight.frag");
        SpotLight = new Shader("lib/renderer/shading/Vertex/Light.vert", "lib/renderer/shading/Fragment/SpotLight.frag");
        AmbientLight = new Shader("lib/renderer/shading/Vertex/Light.vert", "lib/renderer/shading/Fragment/AmbientLight.frag");
        
        DirectionalDepthMap = new Shader("lib/renderer/shading/Vertex/DepthMap.vert", "lib/renderer/shading/Fragment/DirectionalDepthMap.frag");
        OmnidirectionalDepthMap = new Shader("lib/renderer/shading/Vertex/DepthMap.vert", "lib/renderer/shading/Fragment/OmnidirectionalDepthMap.frag", "lib/renderer/shading/Geometry/Omnidirectional.geom");
        VisualizingDepth = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/VisualizingDepth.frag");
     
        EquirectangularToCubemap = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/EquirectangularToCubemap.frag");
        IrradianceConvolution = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/IrradianceConvolution.frag");
        HDRPrefilter = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/HDRPrefilter.frag");
        BRDF = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/BRDF.frag");
        IBL = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/IBL.frag");

        PostProcessing = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/PostProcessing.frag");
        Skybox = new Shader("lib/renderer/shading/Vertex/Skybox.vert", "lib/renderer/shading/Fragment/Skybox.frag");


        unitBox = new Mesh(new BoxGeometry(2, 2, 2));
        screen = new Mesh(new RectangleGeometry(2, 2));
    }


    static void bind() {
        Geometry->bindBlock("View",0);
        PointLight->bindBlock("View", 0);
        RectAreaLight->bindBlock("View", 0);
        SpotLight->bindBlock("View", 0);
        SpotLight->bindBlock("View", 0);
    }


};