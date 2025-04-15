#pragma once

#include "shader.h"
#include "../shading/shaders.h"
#include <glad/glad.h>
#include "framebuffer.h"
#include "../geometry/Rectangular.hpp"
#include "../geometry/3D/RectangularCuboid.h"



/* Objects */
class Objects {
public:
	inline static Rectangular screen;
	inline static RectangularCuboid unitCube;

	static void createObjects() {
		screen = Rectangular(vec3(0), 2, 2);
		unitCube = RectangularCuboid(2.0f, 2.0f, 2.0f);
	}
};



class GaussianBlur {
public:
	inline static FrameBuffer fbo[2];


	static void setup(unsigned int width, unsigned int height) {
		fbo[0] = FrameBuffer(width, height, 0, { {GL_RGBA16F,GL_LINEAR,GL_CLAMP_TO_EDGE}});
		fbo[1] = FrameBuffer(width, height, 0, { {GL_RGBA16F,GL_LINEAR,GL_CLAMP_TO_EDGE}});
	}


	static GLuint blur(GLuint texture, GLuint amount = 10) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		GLuint image = texture;
		bool horizontal = true;
		Shaders::GaussianBlur.use();
		glActiveTexture(GL_TEXTURE0);

		for (int i = 0; i < amount; ++i) {
			fbo[i % 2].bind();
			glBindTexture(GL_TEXTURE_2D, image);
			Shaders::GaussianBlur.setBool("horizontal", horizontal);
			Objects::screen.draw();
			image = fbo[i % 2].buffers[0];
			horizontal = !horizontal;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
		return image;
	}

};



class Bloom {
public:

	static void setup(unsigned int width, unsigned int height) {
		GaussianBlur::setup(width,height);
	}

	static void bloom(GLuint scene , GLuint filterdScene) {
		GLint currentFBO;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

		GLuint blured = GaussianBlur::blur(filterdScene, 10);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blured);
		Shaders::HDRBloom.use();
		Shaders::HDRBloom.setInt("scene",0);
		Shaders::HDRBloom.setInt("bloomBlur", 1);
		Shaders::HDRBloom.setFloat("exposure", 1);
		Objects::screen.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	}

};