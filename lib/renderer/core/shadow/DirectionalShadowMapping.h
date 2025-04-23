#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../framebuffer/FrameBuffer.h"
#include "../shading/Shader.h"
#include "../shading/shaders/Shaders.h"
#include "../3d/Object3d.h"


using namespace std;
using namespace glm;


class DirectionalShadowMapping {
public:
	FrameBuffer fbo;
	mat4 projection, lightMatrix;

	DirectionalShadowMapping(double width, double height, float frustumWidth, float frustumHeight, float frustumDepth) :
		projection(ortho(-frustumWidth / 2.0f, frustumWidth / 2.0f, -frustumHeight / 2.0f, frustumHeight / 2.0f, 0.0f, frustumDepth)),
		fbo(FrameBuffer(width, height, 0, { {GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER} }, true)) {
	}

	DirectionalShadowMapping(double width, double height,float fov, float frustumDepth) :
		projection(perspective(fov, (float)(width / height), 0.001f, frustumDepth)),
		fbo(FrameBuffer(width, height, 0, { {GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER} }, true)) {
	}


	void updateDepthMap(vec3 pos, vec3 direction, vec3 up ,Object3D* root) {
		mat4 view = lookAt(pos, pos + direction, up);
		lightMatrix = projection * view;
		Shaders::DirectionalDepthMap->setMat4("spaceMatrix", lightMatrix);
		glCullFace(GL_FRONT);
		fbo.bind();
		root->render(Shaders::DirectionalDepthMap,mat4(1));
	}


	void visulizeDepthMap(Object3D* root) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Shaders::VisualizingDepth->use();
		Shaders::VisualizingDepth->setMat4("spaceMatrix", lightMatrix);
		root->render(Shaders::VisualizingDepth, mat4(1));
	}


	GLuint getDepthMap() {
		return fbo.buffers[0];
	}

};