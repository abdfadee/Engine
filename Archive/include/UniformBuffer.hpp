#pragma once
#include <glad/glad.h>


class UniformBuffer {
private:
	unsigned int UBO;


public:
	UniformBuffer() {}

	UniformBuffer(unsigned int binding, unsigned int size) {
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, size, 0, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void update(unsigned int offset, unsigned int size, void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

};