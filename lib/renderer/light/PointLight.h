#pragma once
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/SpheroidGeometry.h"
#include "../shadow/OmnidirectionalShadow.h"


class PointLight : public Light {
public:
	OmnidirectionalShadow* shadow;
	vec3 position;
	float radius;

	PointLight(vec3 color , float intensity , float radius) :
	radius(radius),
	shadow(new OmnidirectionalShadow(radius)),
	Light(color,intensity,new SpheroidGeometry(radius,radius,radius)) {}


    virtual void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
		if (!geometeryPass) return;
		worldMatrix = parentMatrix * getLocalMatrix();
		Renderer::lights.push_back(this);

		position = getWorldPosition();
    }


	void bind(Shader *shader, vec3 &viewPos) {
		Light::bind(shader, viewPos);
		
		shader->setVec3("light.position", position);
		shader->setFloat("light.radius", radius);
		shader->setInt("light.type", 1);
		
		float dist = length(position - viewPos);
		if (dist <= radius)
			glCullFace(GL_FRONT);	
		else
			glCullFace(GL_BACK);
		
		shader->setMat4("model", worldMatrix);
		shader->setMat3("normalMatrix", transpose(inverse(mat3(worldMatrix))));

		shader->setFloat("bias", shadow->bias);
		shader->setInt("depthCubeMap", TEXTURE_UNIT_DEPTH_CUBE);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_DEPTH_CUBE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow->getDepthCubeMap());

		draw();
	}

	void updateShadow(Object3D* root) {
		Shaders::OmnidirectionalDepthMap->use();
		shadow->updateDepthCubeMap(position,root);
	}

};