#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../shading/shaders.h"
#include "FrameBuffer.h"
#include "postprocess.h"


using namespace std;
using namespace glm;


class DeferedShading {
public:

	inline static FrameBuffer gBuffer;
	
	static void setup(GLuint width , GLuint height) {
		gBuffer = FrameBuffer(width, height, GL_DEPTH24_STENCIL8, {
			{GL_RGBA16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Position Buffer 
			{GL_RGBA16F,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Normal Buffer 
			{GL_RGB8,GL_NEAREST,GL_CLAMP_TO_EDGE},	// Color Buffer 
		});
	}

	static void generateGeometry (void (*draw)(Shader &shader) , Shader &geometryShader = Shaders::DeferedShadingGeometry) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		gBuffer.bind();
		geometryShader.use();
		draw(geometryShader);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	}

	static void render(Shader &lightingShader , GLuint ssao = -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[2]);

		lightingShader.use();
		lightingShader.setInt("gPosition",0);
		lightingShader.setInt("gNormal", 1);
		lightingShader.setInt("gAlbedo", 2);

		// SSAO
		if (ssao != -1) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, ssao);
			lightingShader.setInt("ssao", 3);
		}

		Objects::screen.draw();
	}

};