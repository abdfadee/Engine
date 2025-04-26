#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../framebuffer/FrameBuffer.h"
#include "../shading/Shader.h"
#include "../shading/Shaders.h"
#include "../Renderer.h"


using namespace std;
using namespace glm;


class OmnidirectionalShadow {
public:
	FrameBuffer fbo;
	float frustumDepth , bias;
	mat4 projection, viewProjectionMatrices[6];

	OmnidirectionalShadow (float frustumDepth,float bias = 1.0f) :
		fbo(FrameBuffer(Renderer::width, Renderer::height, 0, { {GL_DEPTH_COMPONENT24 ,GL_NEAREST ,GL_CLAMP_TO_BORDER,true} }, true)),
		frustumDepth(frustumDepth), bias(bias),
		projection(perspective(radians(90.0f), (float)Renderer::width / (float)Renderer::height, 0.01f, frustumDepth)) {
	}


	void updateDepthCubeMap(vec3 pos,Object3D* root) {
		viewProjectionMatrices[0] = projection * glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		viewProjectionMatrices[1] = projection * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		viewProjectionMatrices[2] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		viewProjectionMatrices[3] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		viewProjectionMatrices[4] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		viewProjectionMatrices[5] = projection * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

		Shaders::OmnidirectionalDepthMap->setMat4("viewProjectionMatrix", mat4(1));
		Shaders::OmnidirectionalDepthMap->setVec3("lightPos", pos);
		Shaders::OmnidirectionalDepthMap->setFloat("farPlane", frustumDepth);
		string prefix = "viewProjectionMatrices", str;
		for (int i = 0; i < 6; i++) {
			str = prefix + "[" + to_string(i) + "]";
			Shaders::OmnidirectionalDepthMap->setMat4(str, viewProjectionMatrices[i]);
		}

		fbo.bind();

		glCullFace(GL_FRONT);
		root->render(Shaders::OmnidirectionalDepthMap);
	}


	void visulizeDepthCubeMap(mat4 view , mat4 projection) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		mat4 mviewProjectionMatrix = projection * mat4(mat3(view));

		Shaders::Skybox->use();
		Shaders::Skybox->setMat4("mviewProjectionMatrix", mviewProjectionMatrix);

		Shaders::Skybox->setInt("cube", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, getDepthCubeMap());

		Shaders::unitBox->render(Shaders::Skybox);
	}

	GLuint getDepthCubeMap() {
		return fbo.buffers[0];
	}

};