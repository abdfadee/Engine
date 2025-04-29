#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <stdexcept>
#include "Vertex.h"

using namespace glm;
using namespace std;


class Geometry {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;

};