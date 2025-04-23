#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../framebuffer/FrameBuffer.h"
#include "../shading/Shader.h"
#include "../shading/shaders/Shaders.h"


using namespace std;
using namespace glm;


class OmnidirectionalShadowMapping {
public:
	FrameBuffer fbo;
	float frustumDepth;
	mat4 projection, spaceMatrices[6];

	OmnidirectionalShadowMapping() {}

	OmnidirectionalShadowMapping(unsigned int width, unsigned int height, float frustumDepth = 10.0f) :
		fbo(FrameBuffer(width, height, 0, { {GL_DEPTH_COMPONENT24 ,GL_NEAREST ,GL_CLAMP_TO_BORDER,true} }, true)),
		frustumDepth(frustumDepth),
		projection(perspective(radians(90.0f), (float)width / (float)height, 0.01f, frustumDepth)) {
	}


	void updateDepthCubeMap(vec3 pos, void (*render)(Shader& shader)) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		spaceMatrices[0] = projection * glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[1] = projection * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[2] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		spaceMatrices[3] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		spaceMatrices[4] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[5] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

		Shaders::OmnidirectionalDepthMap.use();
		Shaders::OmnidirectionalDepthMap.setMat4("spaceMatrix", mat4(1));
		Shaders::OmnidirectionalDepthMap.setVec3("lightPos", pos);
		Shaders::OmnidirectionalDepthMap.setFloat("far_plane", frustumDepth);
		string prefix = "spaceMatrices", str;
		for (int i = 0; i < 6; i++) {
			str = prefix + "[" + to_string(i) + "]";
			Shaders::OmnidirectionalDepthMap.setMat4(str, spaceMatrices[i]);
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
		Shaders::Skybox.setMat4("model", mat4(1));
		Shaders::Skybox.setInt("tex", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.buffers[0]);
		Objects::unitCube.draw();
	}

};