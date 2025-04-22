#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Globals.hpp"
#include "FrameBuffer.h"


using namespace std;
using namespace glm;



class DynamicMapping {

private:
	FrameBuffer fbo;
	mat4 projection , spaceMatrices[6];


public:

	DynamicMapping() {}

	DynamicMapping(unsigned int width, unsigned int height, float frustumDepth, Shader usedShader) :
		fbo(FrameBuffer(width, height, 0, { {GL_RGBA16F ,GL_LINEAR ,GL_CLAMP_TO_EDGE,true},{GL_DEPTH24_STENCIL8 ,GL_NEAREST ,GL_CLAMP_TO_EDGE,true} })),
		projection(perspective(radians(90.0f), (float)width / (float)height, 0.01f, frustumDepth)) {}



	void generateEnv(vec3 pos,void (*draw)(Shader &shader) ) {
		spaceMatrices[0] = projection * glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[1] = projection * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[2] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		spaceMatrices[3] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		spaceMatrices[4] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		spaceMatrices[5] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		fbo.bind();
		Shaders::DynamicMapping.use();
		Shaders::DynamicMapping.setVec3("viewPos", pos);
		string prefix = "spaceMatrices", str;
		for (int i = 0; i < 6; i++) {
			str = prefix + "[" + to_string(i) + "]";
			Shaders::DynamicMapping.setMat4(str, spaceMatrices[i]);
		}
		draw(Shaders::DynamicMapping);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	}


	void bindEnviromentCube(GLuint unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		Shaders::DynamicMapping.setInt("cube",unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.buffers[0]);
	}


	void visulizeCubeMap() {
		Shaders::Skybox.use();
		Shaders::Skybox.setMat4("model", mat4(1));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.buffers[0]);
		Objects::unitCube.draw();
	}

};