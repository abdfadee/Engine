#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>


using namespace std;
using namespace glm;


class Rectangular {
private:
	vector<vec3> vertices;
	vector<vec2> texCoord;
	vector<GLfloat> buffer;
	GLuint VAO, VBO;


public:
	Rectangular() {}
	Rectangular(vec3 origin , float lenght, float height){
		vec3 n = vec3{ 0,0,1 };

		float xoffset = lenght / 2.0f;
		float yoffset = height / 2.0f;
		vertices = {
			{origin.x - xoffset,origin.y - yoffset,origin.z},
			{origin.x + xoffset,origin.y - yoffset,origin.z},
			{origin.x - xoffset,origin.y + yoffset,origin.z},
			{origin.x + xoffset,origin.y - yoffset,origin.z},
			{origin.x - xoffset,origin.y + yoffset,origin.z},
			{origin.x + xoffset,origin.y + yoffset,origin.z},
		};
		texCoord = {
			{0.0f,0.0f},
			{1.0f,0.0f},
			{0.0f,1.0f},
			{1.0f,0.0f},
			{0.0f,1.0f},
			{1.0f,1.0f},
		};
		for (int i = 0; i < vertices.size(); i++) {
			vec3 v = vertices.at(i);
			vec2 t = texCoord.at(i);
			buffer.push_back(v.x);
			buffer.push_back(v.y);
			buffer.push_back(v.z);
			buffer.push_back(n.x);
			buffer.push_back(n.y);
			buffer.push_back(n.z);
			buffer.push_back(t.x);
			buffer.push_back(t.y);
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(sizeof(vec3)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(2*sizeof(vec3)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}



	void draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0,6);
		glBindVertexArray(0);
	}


	void remove() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

};