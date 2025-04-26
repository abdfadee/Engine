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
#include "light/Lighting.h"

using namespace std;


class Renderer {
public:
	FrameBuffer* gBuffer , *lBuffer;
	GLFWwindow* window;
	double deltaTime, currentFrameTime, lastFrameTime;

	inline static GLuint width, height;
	inline static Lighting* lighting;
	

public:
	Renderer (GLFWwindow* window , GLuint width , GLuint height) : 
		window(window) {
		Shaders::compile();
		Shaders::bind();
		Renderer::width = width;
		Renderer::height = height;

		gBuffer = new FrameBuffer(width, height,GL_DEPTH_COMPONENT24, {
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Position Buffer
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Normal Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},		// Color Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Material Buffer 
		});
		lBuffer = new FrameBuffer(width, height, GL_DEPTH_COMPONENT24, {
				{GL_RGBA16F,GL_NEAREST,GL_CLAMP_TO_EDGE}
		});

		lighting = new Lighting(width, height);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}


	void render(Object3D* root, Camera* camera) {
		glViewport(0, 0, width, height);

		camera->updateViewProjectionMatrix();
		vec3 viewPos = camera->getWorldPosition();

		// Geometery Pass
		gBuffer->bind();


		// SkyBox
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		Shaders::Skybox->use();
		Shaders::Skybox->setMat4("mviewProjectionMatrix", camera->projection * mat4(mat3(camera->view)));
		Shaders::Skybox->setInt("cube", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lighting->ibl->maps[0]);
		Shaders::unitBox->render(Shaders::Skybox);


		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		Shaders::Geometry->use();
		Shaders::Geometry->setVec3("viewPos",viewPos);
		Shaders::Geometry->setInt("albedoMap", 0);
		Shaders::Geometry->setInt("roughnessMap", 1);
		Shaders::Geometry->setInt("metallicMap", 2);
		Shaders::Geometry->setInt("normalMap", 3);
		Shaders::Geometry->setInt("displacmentMap", 4);
		Shaders::Geometry->setInt("aoMap", 5);

		root->render(Shaders::Geometry,mat4(1.0f),true,true);



		// Shadow Pass
		lighting->updateDepthMaps(root);



		// Light Pass
		lBuffer->bind();

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		for (int i = 0; i < gBuffer->buffers.size() ; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gBuffer->buffers[i]);
		}
		lighting->illuminate(viewPos);
		lighting->clear();



		// PostProcessing
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Shaders::PostProcessing->use();
		Shaders::PostProcessing->setInt("buffer", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lBuffer->buffers[0]);
		Shaders::screen->render(Shaders::PostProcessing);


		/*
		// Debug
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		Shaders::Basic->use();
		Shaders::Basic->setMat4("viewProjectionMatrix", viewProjectionMatrix);
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