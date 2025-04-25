#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../framebuffer/FrameBuffer.h"
#include "../shading/Shader.h"
#include "../shading/shaders/Shaders.h"
#include "../core/Object3d.h";


using namespace std;
using namespace glm;


class DirectionalShadow {
public:
	FrameBuffer fbo;
	mat4 projection , viewProjectionMatrix;
	float bias;
	bool perspective;

	DirectionalShadow (float frustumWidth, float frustumHeight, float frustumDepth) :
		perspective(false), bias(0.005),
		projection(ortho(-frustumWidth / 2.0f, frustumWidth / 2.0f, -frustumHeight / 2.0f, frustumHeight / 2.0f, 0.0f, frustumDepth)),
		fbo(FrameBuffer(Renderer::width, Renderer::height, 0, { {GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER} }, true)) {}

	DirectionalShadow(float fov, float frustumDepth) :
		perspective(true), bias(0.000005),
		projection(glm::perspective(fov, (float)(Renderer::width / Renderer::height), 0.001f, frustumDepth)),
		fbo(FrameBuffer(Renderer::width, Renderer::height, 0, { {GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER} }, true)) {}


	void updateDepthMap(vec3 pos, vec3 direction, vec3 up ,Object3D* root) {
		mat4 view = lookAt(pos, pos + direction, up);
		viewProjectionMatrix = projection * view;
		Shaders::DirectionalDepthMap->setMat4("viewProjectionMatrix", viewProjectionMatrix);
		glCullFace(GL_FRONT);
		fbo.bind();
		root->render(Shaders::DirectionalDepthMap);
	}


	void visulizeDepthMap() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		Shaders::VisualizingDepth->use();
		Shaders::VisualizingDepth->setInt("depthMap",0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,getDepthMap());
		Shaders::VisualizingDepth->setFloat("znear", 0.001f);
		Shaders::VisualizingDepth->setFloat("zfar", 100.0f);
		Shaders::VisualizingDepth->setBool("perspective",perspective);

		Renderer::screen->render(Shaders::VisualizingDepth);
	}


	GLuint getDepthMap() {
		return fbo.buffers[0];
	}

};