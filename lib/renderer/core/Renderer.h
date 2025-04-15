#pragma once
#include <iostream>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Initiliaze.h"
#include "3d/Object3d.h"
#include "3d/Mesh.h"
#include "view/Camera.h"
#include "shading/shaders/Shaders.h"
#include "../utility/Controls.h"
#include "framebuffer/FrameBuffer.h"
#include "geometry/RectangleGeometry.h"
#include "light/Lighting.h"

using namespace std;


class Renderer {
public:
	GLuint width, height;
	FrameBuffer* gBuffer;
	GLFWwindow* window;
	double deltaTime, currentFrameTime, lastFrameTime;

	inline static Mesh* screen;
	inline static Lighting lighting;
	

public:
	Renderer (GLFWwindow* window , GLuint width , GLuint height) : 
		width(width), height(height), window(window) {
		Shaders::prepare();
		gBuffer = new FrameBuffer(width, height, 0, {
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Normal Buffer 
			{GL_RGB8,GL_NEAREST,GL_CLAMP_TO_EDGE},		// Color Buffer 
			{GL_RGB16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Material Buffer 
			{GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER}	// Depth Buffer
		});
		screen = new Mesh(new RectangleGeometry(2, 2));

		glViewport(0, 0, width, height);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}


	void render(Object3D* root, Camera* camera) {
		camera->updateMatrices();
		vec3 viewPos = camera->getWorldPosition();

		// Geometery Pass
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);

		gBuffer->bind();

		Shaders::Geometry->use();
		Shaders::Geometry->setVec3("viewPos",viewPos);
		Shaders::Geometry->setInt("albedoMap", 0);
		Shaders::Geometry->setInt("roughnessMap", 1);
		Shaders::Geometry->setInt("metallicMap", 2);
		Shaders::Geometry->setInt("normalMap", 3);
		Shaders::Geometry->setInt("displacmentMap", 4);
		Shaders::Geometry->setInt("aoMap", 5);

		root->render(Shaders::Geometry);



		// Light Pass
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		for (int i = 0; i < gBuffer->buffers.size() ; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gBuffer->buffers[i]);
		}

		lighting.illuminate(viewPos);
		lighting.clear();
	}


	void setAnimationLoop(std::function<void()> animationLoop) {
		lastFrameTime = glfwGetTime();

		while (!glfwWindowShouldClose(Renderer::window)) {
			currentFrameTime = glfwGetTime();
			deltaTime = (currentFrameTime - lastFrameTime);
			lastFrameTime = currentFrameTime;

			Controls::processInput(deltaTime);

			animationLoop();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}


};