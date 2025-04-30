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
        * Basic,
        * Geometry,
        * Light,
        * PostProcessing,
        * Skybox,
        * DirectionalDepthMap,
        * VisualizingDepth,
        * OmnidirectionalDepthMap,
        * EquirectangularToCubemap,
        * IrradianceConvolution,
        * HDRPrefilter,
        * BRDF,
        * IBL,
        * GaussianBlur;


    inline static Mesh* screen, * unitBox;



    static void compile() {
        Basic = new Shader("lib/renderer/shading/Vertex/Basic.vert", "lib/renderer/shading/Fragment/Other/Basic.frag");

        Geometry = new Shader("lib/renderer/shading/Vertex/Geometry.vert", "lib/renderer/shading/Fragment/Lighting/Geometry.frag");
        Light = new Shader("lib/renderer/shading/Vertex/Light.vert", "lib/renderer/shading/Fragment/Lighting/Light.frag");
        
        DirectionalDepthMap = new Shader("lib/renderer/shading/Vertex/DepthMap.vert", "lib/renderer/shading/Fragment/Shadow/DirectionalDepthMap.frag");
        OmnidirectionalDepthMap = new Shader("lib/renderer/shading/Vertex/DepthMap.vert", "lib/renderer/shading/Fragment/Shadow/OmnidirectionalDepthMap.frag", "lib/renderer/shading/Geometry/Omnidirectional.geom");
        VisualizingDepth = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/Other/VisualizingDepth.frag");
     
        EquirectangularToCubemap = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/IBL/EquirectangularToCubemap.frag");
        IrradianceConvolution = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/IBL/IrradianceConvolution.frag");
        HDRPrefilter = new Shader("lib/renderer/shading/Vertex/IBL.vert", "lib/renderer/shading/Fragment/IBL/HDRPrefilter.frag");
        BRDF = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/IBL/BRDF.frag");
        IBL = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/IBL/IBL.frag");

        Skybox = new Shader("lib/renderer/shading/Vertex/Skybox.vert", "lib/renderer/shading/Fragment/Lighting/Skybox.frag");
        PostProcessing = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/Tech/PostProcessing.frag");

        GaussianBlur = new Shader("lib/renderer/shading/Vertex/PostProcessing.vert", "lib/renderer/shading/Fragment/Tech/GaussianBlur.frag");

        unitBox = new Mesh(new BoxGeometry(2, 2, 2));
        screen = new Mesh(new RectangleGeometry(2, 2));
    }


    static void bind() {
        Geometry->bindBlock("View",0);
        
    }


};