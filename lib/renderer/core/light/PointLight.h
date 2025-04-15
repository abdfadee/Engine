#pragma once
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/SpheroidGeometry.h"


class PointLight : public Light {
	float radius;

public:
	PointLight(vec3 color , float intensity , float radius) :
	radius(radius),
	Light(color,intensity,new SpheroidGeometry(radius,radius,radius)) {}


    virtual void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = true) {
		worldMatrix = parentMatrix * getLocalMatrix();
		Renderer::lighting.pointLights.push_back(this);
    }


	void bind(Shader *shader, vec3 &viewPos) {
		Light::bind(shader, viewPos);
		vec3 position = getWorldPosition();
		shader->setVec3("light.position", position);
		shader->setFloat("light.radius", radius);
		
		float dist = length(position - viewPos);
		if (dist <= radius + 0.1)
			glCullFace(GL_FRONT);	
		else
			glCullFace(GL_BACK);
		
		shader->setMat4("model", worldMatrix);
		shader->setMat3("normalMatrix", transpose(inverse(mat3(worldMatrix))));
		draw();
	}

};