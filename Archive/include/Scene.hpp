#pragma once
#include "Shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Globals.hpp"
#include "Textures.hpp"
#include "../Model/Model.h"
#include "DynamicMapping.hpp"
#include "../Shapes/3D/Spheroid.h"
#include "../Shapes/3D/Pipe.h"
#include "../Shapes/3D/RectangularCuboid.h"
#include "../Shapes/3D/EllipticalCylinder.h"
#include "../Shapes/3D/Cone.h"
#include "../Shapes/3D/Torus.h"
#include "../Properties/Light.h"
#include "FrameBuffer.h"
#include "Camera.hpp"
#include "PostProcess.h"
#include "DeferedShading.h"
#include "SSAO.h"
#include "IBL.h"



using namespace std;
using namespace glm;


extern Camera camera;



class Scene {
private:
	inline static RectangularCuboid rc;
	inline static Spheroid s;
	inline static EllipticalCylinder c;
	inline static Pipe p;
	inline static Cone cone;
	inline static Torus t;
	inline static GLuint envCubemap, irradianceMap , prefilterMap , brdfLUTTexture;


public:
	static void prepare() {
		initilizeObjects();
		IBL::setup();
		envCubemap = IBL::HDRToEnv(Textures::getTexture("container_free_Ref.hdr"));
		irradianceMap = IBL::generateIrradianceCube(envCubemap);
		prefilterMap = IBL::preFilter(envCubemap);
		brdfLUTTexture = IBL::generateBRDFLUT();
	}


	static void initilizeObjects() {
		rc = RectangularCuboid(1.0f, 1.0f, 1.0f,32,32,32);
		s = Spheroid(0.5f,0.5f,0.5f);
		c = EllipticalCylinder(1.0f,1.0f,2.0f,128);
		cone = Cone(0.5f,1.0f);
		p = Pipe(0.25f,0.5f,1.0f);
		t = Torus(1.0f,0.5f);
	}


	static void draw() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,1024,1024);

		camera.update();

		
		Shaders::Skybox.use();
		Shaders::Skybox.setMat4("model",mat4(1));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		Objects::unitCube.draw();


		Shaders::PBR_IBL_TEXTURE.use();

		addLights(Shaders::PBR_IBL_TEXTURE);

		Shaders::PBR_IBL_TEXTURE.setMat4("projection", camera.perspect);
		Shaders::PBR_IBL_TEXTURE.setMat4("view", camera.view);
		Shaders::PBR_IBL_TEXTURE.setVec3("camPos", camera.cameraPos);


		Shaders::PBR_IBL_TEXTURE.setFloat("heightScale", 0.25f);
		
		Shaders::PBR_IBL_TEXTURE.setInt("albedoMap", 0);
		Shaders::PBR_IBL_TEXTURE.setInt("normalMap", 1);
		Shaders::PBR_IBL_TEXTURE.setInt("metallicMap", 2);
		Shaders::PBR_IBL_TEXTURE.setInt("roughnessMap", 3);
		Shaders::PBR_IBL_TEXTURE.setInt("aoMap", 4);
		Shaders::PBR_IBL_TEXTURE.setInt("displacmentMap", 5);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Textures::getTexture("PavingStones138_2K-JPG_Color.jpg"));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Textures::getTexture("PavingStones138_2K-JPG_NormalGL.jpg"));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Textures::getTexture("PavingStones138_2K-JPG_Roughness.jpg"));
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, Textures::getTexture("PavingStones138_2K-JPG_AmbientOcclusion.jpg"));
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, Textures::getTexture("PavingStones138_2K-JPG_Displacement.jpg"));
		

		Shaders::PBR_IBL_TEXTURE.setInt("irradianceMap", 6);
		Shaders::PBR_IBL_TEXTURE.setInt("prefilterMap", 7);
		Shaders::PBR_IBL_TEXTURE.setInt("brdfLUT", 8);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);


		Shaders::PBRIBL.setFloat("metallic", 1.0f);
		Shaders::PBRIBL.setFloat("roughness", 0.2f);
		Shaders::PBRIBL.setVec3("albedo", 1.0f, 0.0f, 0.0f);
		Shaders::PBRIBL.setFloat("ao", 1.0f);

		mat4 model = mat4(1);
		Shaders::PBR_IBL_TEXTURE.setMat3("normalMatrix", transpose(inverse(mat3(model))));
		Shaders::PBR_IBL_TEXTURE.setMat4("model", model);
		rc.draw();

	}



	static void addLights(Shader &shader) {
		glm::vec3 lightPositions[] = {
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(-2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -2.0f, 0.0f),
		};
		glm::vec3 lightColors[] = {
			glm::vec3(5.0f),
			glm::vec3(5.0f),
			glm::vec3(5.0f),
			glm::vec3(5.0f),
		};

		for (unsigned int i = 0; i < 4; ++i)
		{
			shader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
			shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
		}
	}

};