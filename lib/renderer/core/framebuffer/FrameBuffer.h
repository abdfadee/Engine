#pragma once
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include "Formats.h"


using namespace std;


class Buffer {
public:
	GLint internalFormat;
	GLint filterOptions;
	GLint wrapOptions;
	bool multiFace;

	Buffer(GLint internalFormat, GLint filterOptions, GLint wrapOptions,bool multiFace = false)
		: internalFormat(internalFormat), filterOptions(filterOptions), wrapOptions(wrapOptions) , multiFace(multiFace) {}
};


class FrameBuffer {
public:
	GLuint FBO, RBO, width, height;
	vector<GLuint> buffers;


	// Using Render Buffer Object
	FrameBuffer(GLuint width, GLuint height, GLint depth_stencil_internalFormat, vector<Buffer> bufferConfigs , bool noColorBuffer = false) :
		width(width), height(height)
	{
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);


		// Buffer Attachments
		buffers.resize(bufferConfigs.size());
		glGenTextures(bufferConfigs.size(), buffers.data());


		if (noColorBuffer) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		GLuint colorAttachmentIndex = 0;
		for (size_t i = 0; i < bufferConfigs.size(); ++i) {
			GLenum format, type , attachment;
			GetBaseFormatTypeAndAttachment(bufferConfigs[i].internalFormat, format, type,attachment);

			GLenum target;
			if (bufferConfigs[i].multiFace) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, buffers[i]);
				for (int j = 0; j < 6; j++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, bufferConfigs[i].internalFormat, width, height, 0, format, type, nullptr);
				setFilterOptions(GL_TEXTURE_CUBE_MAP, bufferConfigs[i]);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				if (attachment == GL_COLOR_ATTACHMENT0)
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex++, buffers[i], 0);
				else
					glFramebufferTexture(GL_FRAMEBUFFER, attachment, buffers[i], 0);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, buffers[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, bufferConfigs[i].internalFormat, width, height, 0, format, type, nullptr);
				setFilterOptions(GL_TEXTURE_2D, bufferConfigs[i]);
				glBindTexture(GL_TEXTURE_2D, 0);
				if (attachment == GL_COLOR_ATTACHMENT0)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex++, GL_TEXTURE_2D, buffers[i], 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, buffers[i], 0);
			}
			
		}

		vector<GLenum> attachments(colorAttachmentIndex);
		for (size_t i = 0; i < colorAttachmentIndex; ++i)
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(attachments.size(), attachments.data());


		// Depth & Stencil Buffer Object
		if (depth_stencil_internalFormat != 0) {
			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			GLenum depth_stencil_format, depth_stencil_type, depth_stencil_attachment;
			GetBaseFormatTypeAndAttachment(depth_stencil_internalFormat, depth_stencil_format, depth_stencil_type, depth_stencil_attachment);
			glRenderbufferStorage(GL_RENDERBUFFER, depth_stencil_internalFormat, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, depth_stencil_attachment, GL_RENDERBUFFER, RBO);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "ERROR::FRAMEBUFFER:: framebuffer is not complete!" << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void setFilterOptions(GLenum target , const Buffer &bufferConfig) {
		// Apply filtering options
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, bufferConfig.filterOptions);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, bufferConfig.filterOptions);

		// Apply wrapping options
		glTexParameteri(target, GL_TEXTURE_WRAP_S, bufferConfig.wrapOptions);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, bufferConfig.wrapOptions);

		if (bufferConfig.wrapOptions == GL_CLAMP_TO_BORDER) {
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
	}



	void bind(GLint x = 0, GLint y = 0) {
		glViewport(x, y, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}


	void copy(GLuint distBuffer,GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,GLenum filter = GL_NEAREST) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER,FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, distBuffer);
		glBlitFramebuffer(0,0,width,height,0,0,width,height,mask,filter);
	}

};