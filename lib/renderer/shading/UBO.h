#pragma once
#include <glad/glad.h>


class UBO {
private:
	unsigned int id;


public:
	UBO(unsigned int binding, unsigned int size) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_UNIFORM_BUFFER, id);
		glBufferData(GL_UNIFORM_BUFFER, size, 0, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void upload(unsigned int offset, unsigned int size, void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, id);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

};