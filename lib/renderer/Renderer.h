#pragma once
#include <iostream>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Initiliaze.h"
#include "view/Camera.h"
#include "shading/Shaders.h"
#include "utility/Controls.h"
#include "framebuffer/FrameBuffer.h"
#include "lib/renderer/model/Model.h";
#include "light/Light.h";
#include "light/IBL.h";
#include "technique/GaussianBlur.h";


const int TEXTURE_UNIT_SKYBOX = 0;
const int TEXTURE_UNIT_GPosition = 0;
const int TEXTURE_UNIT_GNORMAL = 1;
const int TEXTURE_UNIT_GALBEDO = 2;
const int TEXTURE_UNIT_GMATERIAL = 3;
const int TEXTURE_UNIT_GEMISSIVE = 4;
const int TEXTURE_UNIT_DEPTH = 5;
const int TEXTURE_UNIT_DEPTH_CUBE = 6;
const int TEXTURE_UNIT_IRRADIANCE = 5;
const int TEXTURE_UNIT_PREFILTER = 6;
const int TEXTURE_UNIT_BRDF = 7;


class Renderer {
public:
	FrameBuffer* gBuffer , *lBuffer;
	GLFWwindow* window;
	IBL* ibl;
	GaussianBlur* gaussianBlur;
	
	vec2 pixelSize;
	double deltaTime, currentFrameTime, lastFrameTime;

	inline static GLuint width, height;
	inline static vector<Light*> lights;
	

public:
	Renderer (GLFWwindow* window , GLuint width , GLuint height) : 
		window(window) , ibl(new IBL()) , gaussianBlur(new GaussianBlur(width, height)) {
		Shaders::compile();
		Shaders::bind();
		Renderer::width = width;
		Renderer::height = height;

		pixelSize = vec2(1.0f / width, 1.0f / height);

		gBuffer = new FrameBuffer(width, height,GL_DEPTH_COMPONENT32, {
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Position Buffer
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Normal Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Color Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Material Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Emissive Buffer 
		});
		lBuffer = new FrameBuffer(width, height, GL_DEPTH_COMPONENT32, {
				{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},
				{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE}
		});

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}




	void render(Object3D* root, Camera* camera) {
		glViewport(0, 0, width, height);

		camera->updateViewProjectionMatrix();
		vec3 viewPos = camera->getWorldPosition();




		/* Geometery Pass */
		gBuffer->bind();


		// SkyBox
		Shaders::Skybox->use();
		Shaders::Skybox->setMat4("mviewProjectionMatrix", camera->projection * mat4(mat3(camera->view)));
		Shaders::Skybox->setInt("cube", TEXTURE_UNIT_SKYBOX);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_SKYBOX);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->maps[0]);
		Shaders::unitBox->render(Shaders::Skybox);


		// Rendering
		glEnable(GL_DEPTH_TEST);
		Shaders::Geometry->use();
		Shaders::Geometry->setVec3("viewPos",viewPos);
		Shaders::Geometry->setInt("material.textureAlbedo", TEXTURE_UNIT_ALBEDO);
		Shaders::Geometry->setInt("material.textureNormal", TEXTURE_UNIT_NORMAL);
		Shaders::Geometry->setInt("material.textureHeight", TEXTURE_UNIT_Height);
		Shaders::Geometry->setInt("material.textureMetallicRoughness", TEXTURE_UNIT_METALLIC_ROUGHNESS);
		Shaders::Geometry->setInt("material.textureMetallic", TEXTURE_UNIT_METALLIC);
		Shaders::Geometry->setInt("material.textureRoughness", TEXTURE_UNIT_ROUGHNESS);
		Shaders::Geometry->setInt("material.textureAmbientOcclusion", TEXTURE_UNIT_AMBIENT_OCCLUSION);
		Shaders::Geometry->setInt("material.textureEmissive", TEXTURE_UNIT_EMISSIVE);
		root->render(Shaders::Geometry,mat4(1.0f),true,true);



		
		/* Shadow Pass */
		glEnable(GL_CULL_FACE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f, 4.0f); // tweak these

		for (int i = 0; i < lights.size(); ++i)
			lights[i]->updateShadow(root);

		glDisable(GL_POLYGON_OFFSET_FILL);




		/* Light Pass */
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		lBuffer->bind();
		for (int i = 0; i < gBuffer->buffers.size() ; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gBuffer->buffers[i]);
		}


		// Lights
		Shaders::Light->use();
		Shaders::Light->setVec3("viewPos", viewPos);
		Shaders::Light->setVec2("pixelSize", pixelSize);
		Shaders::Light->setInt("gPosition", TEXTURE_UNIT_GPosition);
		Shaders::Light->setInt("gNormal", TEXTURE_UNIT_GNORMAL);
		Shaders::Light->setInt("gAlbedo", TEXTURE_UNIT_GALBEDO);
		Shaders::Light->setInt("gMaterial", TEXTURE_UNIT_GMATERIAL);
		Shaders::Light->setInt("depthMap", TEXTURE_UNIT_DEPTH);
		Shaders::Light->setInt("depthCubeMap", TEXTURE_UNIT_DEPTH_CUBE);

		for (int i = 0; i < lights.size(); ++i)
			lights[i]->bind(Shaders::Light, viewPos);

		glDisable(GL_CULL_FACE);
		lights.clear();

		
		// IBL
		Shaders::IBL->use();
		Shaders::IBL->setVec3("viewPos", viewPos);
		Shaders::IBL->setVec2("pixelSize", pixelSize);
		Shaders::IBL->setInt("gPosition", TEXTURE_UNIT_GPosition);
		Shaders::IBL->setInt("gNormal", TEXTURE_UNIT_GNORMAL);
		Shaders::IBL->setInt("gAlbedo", TEXTURE_UNIT_GALBEDO);
		Shaders::IBL->setInt("gMaterial", TEXTURE_UNIT_GMATERIAL);
		Shaders::IBL->setInt("gEmissive", TEXTURE_UNIT_GEMISSIVE);
		Shaders::IBL->setInt("irradianceMap", TEXTURE_UNIT_IRRADIANCE);
		Shaders::IBL->setInt("prefilterMap", TEXTURE_UNIT_PREFILTER);
		Shaders::IBL->setInt("brdfLUT", TEXTURE_UNIT_BRDF);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_IRRADIANCE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->maps[1]);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_PREFILTER);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->maps[2]);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_BRDF);
		glBindTexture(GL_TEXTURE_2D, ibl->maps[3]);
		Shaders::screen->render(Shaders::IBL);
		



		/* PostProcessing */
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		
		// GaussianBlur
		GLuint blured = gaussianBlur->blur(lBuffer->buffers[1], 10);


		// HDR + Bloom
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Shaders::PostProcessing->use();
		Shaders::PostProcessing->setInt("scene", 0);
		Shaders::PostProcessing->setInt("blur", 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lBuffer->buffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blured);
		Shaders::screen->render(Shaders::PostProcessing);




		/*
		// Debug
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		Shaders::Basic->use();
		root->render(Shaders::Basic);
		*/
		
	}




	void setAnimationLoop(std::function<void(float)> animationLoop) {
		lastFrameTime = glfwGetTime();

		while (!glfwWindowShouldClose(Renderer::window)) {
			currentFrameTime = glfwGetTime();
			deltaTime = (currentFrameTime - lastFrameTime);
			lastFrameTime = currentFrameTime;

			Controls::processInput(deltaTime);

			animationLoop(deltaTime);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}


};