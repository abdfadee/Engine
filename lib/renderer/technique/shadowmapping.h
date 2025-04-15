#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../shading/shaders.h"
#include "framebuffer.h"
#include "postprocess.h"


using namespace std;
using namespace glm;


class DirectionalShadowMapping {
public:
	FrameBuffer fbo;
	mat4 projection , lightMatrix;

	DirectionalShadowMapping() {}
	DirectionalShadowMapping(unsigned int width, unsigned int height, float frustumWidth = 10.0f , float frustumHeight = 10.0f, float frustumDepth = 10.0f) :
		projection(ortho(-frustumWidth/2.0f, frustumWidth / 2.0f, -frustumHeight/2.0f, frustumHeight / 2.0f, 0.0f, frustumDepth)),
		fbo(FrameBuffer(width, height, 0, { {GL_DEPTH_COMPONENT24,GL_NEAREST,GL_CLAMP_TO_BORDER} }, true))  {}


	void updateDepthMap(vec3 pos, vec3 direction, void(*render)(Shader& shader)) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		vec3 look = normalize(pos + direction);
		vec3 right = cross(look, vec3(0, 1, 0));
		vec3 up = cross(right, look);
		mat4 view = lookAt(pos, pos + direction , up);
		lightMatrix = projection * view;
		Shaders::DirectionalDepthMap.use();
		Shaders::DirectionalDepthMap.setMat4("spaceMatrix", lightMatrix);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		fbo.bind();
		render(Shaders::DirectionalDepthMap);
		glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	}


	void visulizeDepthMap(void(*render)(Shader &shader)) {
		Shaders::VisualizingDepth.use();
		Shaders::VisualizingDepth.setMat4("spaceMatrix",lightMatrix);
		render(Shaders::VisualizingDepth);
	}


};



class OmnidirectionalShadowMapping {
public:
	FrameBuffer fbo;
	float frustumDepth;
	mat4 projection , spaceMatrices[6];

	OmnidirectionalShadowMapping() {}

	OmnidirectionalShadowMapping(unsigned int width, unsigned int height, float frustumDepth = 10.0f) :
		fbo(FrameBuffer(width, height, 0, { {GL_DEPTH_COMPONENT24 ,GL_NEAREST ,GL_CLAMP_TO_BORDER,true} }, true)),
		frustumDepth(frustumDepth),
		projection(projectionive(radians(90.0f), (float)width / (float)height, 0.01f, frustumDepth)) {}


	void updateDepthCubeMap(vec3 pos , void (*render)(Shader& shader) ) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		spaceMatrices[0] = projection * glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[1] = projection * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[2] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		spaceMatrices[3] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		spaceMatrices[4] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[5] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
		
		Shaders::OmnidirectionalDepthMap.use();
		Shaders::OmnidirectionalDepthMap.setMat4("spaceMatrix",mat4(1));
		Shaders::OmnidirectionalDepthMap.setVec3("lightPos",pos);
		Shaders::OmnidirectionalDepthMap.setFloat("far_plane",frustumDepth);
		string prefix = "spaceMatrices", str;
		for (int i = 0; i < 6; i++) {
			str = prefix + "[" + to_string(i) + "]";
			Shaders::OmnidirectionalDepthMap.setMat4(str,spaceMatrices[i]);
		}

		fbo.bind();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		render(Shaders::OmnidirectionalDepthMap);
		glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	}


	void visulizeDepthCubeMap() {
		Shaders::Skybox.use();
		Shaders::Skybox.setMat4("model",mat4(1));
		Shaders::Skybox.setInt("tex", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.buffers[0]);
		Objects::unitCube.draw();
	}

};