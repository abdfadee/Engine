#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "Textures.hpp"
#include "../Shapes/Rectangular.hpp"
#include "../Shapes/3D/RectangularCuboid.h"


using namespace glm;


/* Shaders */
class Shaders {
public:
    inline static Shader
        Basic,
        Phong,
        PhongCasters,
        Model,
        PostProcess,
        Cubemap,
        Skybox,
        EnviromentMapping,
        DynamicMapping,
        Normals,
        BlinnPhong,
        ShadowMapping,
        DirectionalDepthMap,
        VisualizingDepth,
        OmnidirectionalDepthMap,
        BlinnPhongCastersShadows,
        NormalMapping,
        ParallaxMapping,
        GaussianBlur,
        HDRBloom,
        DeferedShadingGeometry,
        DeferedShadingLighting,
        DeferedShadingGeometryViewSpace,
        SSAO,
        SSAOBlur,
        SSAOLighting,
        PBR,
        EquirectangularToCubemap,
        IrradianceConvolution,
        PBRIBL,
        HDRPrefilter,
        BRDF,
        PBR_IBL_TEXTURE;



    static void compile() {
        Basic = Shader("Shaders/Vertex/Basic.vert", "Shaders/Fragment/Basic.frag");
        Phong = Shader("Shaders/Vertex/Lighting.vert", "Shaders/Fragment/PhongLighting.frag");
        PhongCasters = Shader("Shaders/Vertex/Lighting.vert", "Shaders/Fragment/PhongLightCasters.frag");
        Model = Shader("Shaders/Vertex/Lighting.vert", "Shaders/Fragment/Model.frag");
        PostProcess = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/Post-Processing.frag");
        Cubemap = Shader("Shaders/Vertex/Cubemap.vert", "Shaders/Fragment/Cubemap.frag");
        Skybox = Shader("Shaders/Vertex/Skybox.vert", "Shaders/Fragment/Cubemap.frag");
        EnviromentMapping = Shader("Shaders/Vertex/Lighting.vert", "Shaders/Fragment/EnviromentMapping.frag");
        DynamicMapping = Shader("Shaders/Vertex/WorldSpace.vert", "Shaders/Fragment/Basic.frag", "Shaders/Geometry/DynamicMapping.geom");
        Normals = Shader("Shaders/Vertex/Normals.vert","Shaders/Fragment/Basic.frag", "Shaders/Geometry/NormalsGenerator.geom");
        BlinnPhong = Shader("Shaders/Vertex/Lighting.vert", "Shaders/Fragment/Blinn-Phong.frag");
        ShadowMapping = Shader("Shaders/Vertex/ShadowMapping.vert", "Shaders/Fragment/ShadowMapping.frag");
        DirectionalDepthMap = Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/DirectionalDepthMap.frag");
        VisualizingDepth = Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/VisualizingDepth.frag");
        OmnidirectionalDepthMap = Shader("Shaders/Vertex/DepthMap.vert", "Shaders/Fragment/OmnidirectionalDepthMap.frag", "Shaders/Geometry/Omnidirectional.geom");
        BlinnPhongCastersShadows = Shader("Shaders/Vertex/Blinn-Phong-Casters-Shadows.vert", "Shaders/Fragment/Blinn-Phong-Casters-Shadows.frag");
        NormalMapping = Shader("Shaders/Vertex/NormalMapping.vert", "Shaders/Fragment/NormalMapping.frag"),
        ParallaxMapping = Shader("Shaders/Vertex/NormalMapping.vert", "Shaders/Fragment/ParallaxMapping.frag");
        GaussianBlur = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/GaussianBlur.frag");
        HDRBloom = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/HDRBloom.frag"),
        DeferedShadingGeometry = Shader("Shaders/Vertex/DeferedShadingGeometry.vert", "Shaders/Fragment/DeferedShadingGeometry.frag");
        DeferedShadingLighting = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/DeferedShadingLighting.frag");
        DeferedShadingGeometryViewSpace = Shader("Shaders/Vertex/DeferedShadingGeometryViewSpace.vert", "Shaders/Fragment/DeferedShadingGeometry.frag");
        SSAO = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/SSAO.frag");
        SSAOBlur = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/SSAOBlur.frag");
        SSAOLighting = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/SSAOLighting.frag");
        PBR = Shader("Shaders/Vertex/PBR.vert", "Shaders/Fragment/PBR.frag");
        EquirectangularToCubemap = Shader("Shaders/Vertex/IBL.vert", "Shaders/Fragment/EquirectangularToCubemap.frag");
        IrradianceConvolution = Shader("Shaders/Vertex/IBL.vert", "Shaders/Fragment/IrradianceConvolution.frag");
        PBRIBL = Shader("Shaders/Vertex/PBR.vert", "Shaders/Fragment/PBRIBL.frag");
        HDRPrefilter = Shader("Shaders/Vertex/IBL.vert", "Shaders/Fragment/HDRPrefilter.frag");
        BRDF = Shader("Shaders/Vertex/Post-Processing.vert", "Shaders/Fragment/BRDF.frag");
        PBR_IBL_TEXTURE = Shader("Shaders/Vertex/PBR.vert", "Shaders/Fragment/PBR_IBL_TEXTURE.frag");
    }


    static void bindBuffers() {
        Basic.bindBlock("CameraBuffer", 0);
        Phong.bindBlock("CameraBuffer", 0);
        PhongCasters.bindBlock("CameraBuffer", 0);
        Model.bindBlock("CameraBuffer", 0);
        Cubemap.bindBlock("CameraBuffer", 0);
        Skybox.bindBlock("CameraBuffer", 0);
        EnviromentMapping.bindBlock("CameraBuffer", 0);
        BlinnPhong.bindBlock("CameraBuffer", 0);
        ShadowMapping.bindBlock("CameraBuffer", 0);
        BlinnPhongCastersShadows.bindBlock("CameraBuffer", 0);
        NormalMapping.bindBlock("CameraBuffer", 0);
        DeferedShadingGeometry.bindBlock("CameraBuffer", 0);
        DeferedShadingGeometryViewSpace.bindBlock("CameraBuffer", 0);
    }
};



/* Uniform Buffers */
class UniformBuffers {
public:
    inline static UniformBuffer cameraBuffer;

    static void generate() {
        cameraBuffer = UniformBuffer(0, 32 * sizeof(float));
    }
};



/* Objects */
class Objects {
public:
    inline static Rectangular screen;
    inline static RectangularCuboid unitCube;

    static void createObjects() {
        screen = Rectangular(vec3(0), 2, 2);
        unitCube = RectangularCuboid(2.0f,2.0f,2.0f);
    }
};



/* Textures */
static void loadTextures() {
    //Textures::addTexture("Assets/2D/c.png");
    //Textures::addTexture("Assets/2D/brickwall.jpg");
    //Textures::addTexture("Assets/2D/brickwall_normal.jpg", false);
    //Textures::addTexture("Assets/2D/bricks2.jpg");
    //Textures::addTexture("Assets/2D/bricks2_disp.jpg", false);
    //Textures::addTexture("Assets/2D/bricks2_normal.jpg", false);

    //Textures::addTexture("Assets/PBR/rustediron/rustediron2_basecolor.png", false);
    //Textures::addTexture("Assets/PBR/rustediron/rustediron2_metallic.png", false);
    //Textures::addTexture("Assets/PBR/rustediron/rustediron2_normal.png", false);
    //Textures::addTexture("Assets/PBR/rustediron/rustediron2_roughness.png", false);

    Textures::addTexture("Assets/PBR/PavingStones/PavingStones138_2K-JPG_Color.jpg", false);
    Textures::addTexture("Assets/PBR/PavingStones/PavingStones138_2K-JPG_NormalGL.jpg", false);
    Textures::addTexture("Assets/PBR/PavingStones/PavingStones138_2K-JPG_Displacement.jpg", false);
    Textures::addTexture("Assets/PBR/PavingStones/PavingStones138_2K-JPG_Roughness.jpg", false);
    Textures::addTexture("Assets/PBR/PavingStones/PavingStones138_2K-JPG_AmbientOcclusion.jpg", false);
    

    //Textures::addTexture("Assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Color.jpg", false);
    //Textures::addTexture("Assets/PBR/PavingStones115/PavingStones115B_1K-JPG_NormalGL.jpg", false);
    //Textures::addTexture("Assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Displacement.jpg", false);
    //Textures::addTexture("Assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Roughness.jpg", false);
    //Textures::addTexture("Assets/PBR/PavingStones115/PavingStones115B_1K-JPG_AmbientOcclusion.jpg", false);


    //Textures::addTexture("Assets/PBR/Metal/Metal049A_2K-JPG_Color.jpg", false);
    //Textures::addTexture("Assets/PBR/Metal/Metal049A_2K-JPG_NormalGL.jpg", false);
    //Textures::addTexture("Assets/PBR/Metal/Metal049A_2K-JPG_Metalness.jpg", false);
    //Textures::addTexture("Assets/PBR/Metal/Metal049A_2K-JPG_Roughness.jpg", false);
    //Textures::addTexture("Assets/PBR/Metal/Metal049A_2K-JPG_Displacement.jpg", false);


    Textures::addHDRI("Assets/HDRI/container_free_Ref.hdr");
    Textures::addHDRI("Assets/HDRI/satara_night_no_lamps_2k.hdr");

    Textures::addCubeMap("Assets/cube_maps/skybox");

}



/* OpenGL Configuration	*/
static void configure() {

    /* PolygonMode */
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);


    /* Depth Test */
    glEnable(GL_DEPTH_TEST);
    //glDepthRange(0,1);
    //glDepthMask(0xff);
    glDepthFunc(GL_LEQUAL);


    /* Stencil Test */
    //glEnable(GL_STENCIL_TEST);
    //glStencilMask(0xff);
    //glStencilFunc(GL_ALWAYS,0,0xff);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


    // Blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFuncSeparate(GL_SRC_COLOR,GL_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);


    // Face Culling
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glFrontFace(GL_CCW)


    // Anti-Aliasing
    //glfwWindowHint(GLFW_SAMPLES, 4);
    //glEnable(GL_MULTISAMPLE);



    // Gama Corection
    //glEnable(GL_FRAMEBUFFER_SRGB);

}




void initializeEngine() {
    Shaders::compile();
    Shaders::bindBuffers();
    UniformBuffers::generate();
    Objects::createObjects();
    loadTextures();
    configure();
}