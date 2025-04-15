#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <stdexcept>

using namespace glm;
using namespace std;


class Geometry {
public:
	vector<vec3> vertices, normals, tangents, bitangents;
	vector<vec2> uv;
	
	vector<GLfloat> buffer;
    vector<unsigned int> indices;


    void generateBuffer() {
        if (vertices.size() != normals.size() ||
            vertices.size() != uv.size() ||
            vertices.size() != tangents.size() ||
            vertices.size() != bitangents.size()) {
            throw std::runtime_error("Geometry data arrays must have the same size!");
        }

        buffer.clear();
        buffer.reserve(vertices.size() * (3 + 3 + 2 + 3 + 3));

        for (size_t i = 0; i < vertices.size(); ++i) {
            buffer.push_back(vertices[i].x);
            buffer.push_back(vertices[i].y);
            buffer.push_back(vertices[i].z);

            buffer.push_back(normals[i].x);
            buffer.push_back(normals[i].y);
            buffer.push_back(normals[i].z);

            buffer.push_back(uv[i].x);
            buffer.push_back(uv[i].y);

            buffer.push_back(tangents[i].x);
            buffer.push_back(tangents[i].y);
            buffer.push_back(tangents[i].z);

            buffer.push_back(bitangents[i].x);
            buffer.push_back(bitangents[i].y);
            buffer.push_back(bitangents[i].z);
        }
    }
};