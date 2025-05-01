#pragma once
#include <glad/glad.h>
#include "../shading/Shaders.h"
#include "../framebuffer/FrameBuffer.h"



class GaussianBlur {
public:
	FrameBuffer *fbo[2];


	GaussianBlur(unsigned int width, unsigned int height) {
		fbo[0] = new FrameBuffer(width, height, 0, { {GL_RGBA16F,GL_LINEAR,GL_CLAMP_TO_EDGE} });
		fbo[1] = new FrameBuffer(width, height, 0, { {GL_RGBA16F,GL_LINEAR,GL_CLAMP_TO_EDGE} });
	}


	GLuint blur(GLuint texture, GLuint amount = 10) {
		GLuint image = texture;
		bool horizontal = true;
		Shaders::GaussianBlur->use();
		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < amount; ++i) {
			fbo[i % 2]->bind();
			glBindTexture(GL_TEXTURE_2D, image);
			Shaders::GaussianBlur->setBool("horizontal", horizontal);
			Shaders::screen->render(Shaders::GaussianBlur);
			image = fbo[i % 2]->buffers[0];
			horizontal = !horizontal;
		}
		return image;
	}

};