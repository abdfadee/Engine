#pragma once
#include <glm/glm.hpp>
#include "../3d/Mesh.h"

using namespace glm;


class Light : public Mesh {
public:
	vec3 color;
	float intensity;

	Light(vec3 color, float intensity , Geometry* geometry) :
		color(color),
		intensity(intensity), 
		Mesh(geometry) {}


	virtual void bind(Shader *shader,vec3 &viewPos) {
		shader->setVec3("light.color", color);
		shader->setFloat("light.intensity",intensity);
	}

};