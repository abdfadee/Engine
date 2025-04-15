#pragma once
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/ConeGeometry.h"
#include "../../utility/helpers.h"
#include "PointLight.h"


class SpotLight : public Light{
	float innerCutOff, outerCutOff, distance , baseRadius , cosTheta2;
	vec3 rotation, position;
	PointLight* ambientLight;

public:
	SpotLight(vec3 color, float intensity, vec3 direction, float outerCutOff, float innerCutOff, float distance) :
		distance(distance),baseRadius(distance * tan(outerCutOff)),
		Light(color, intensity, new ConeGeometry(baseRadius, distance)),
		ambientLight(new PointLight(color, intensity, distance * 2)),
		outerCutOff(cos(outerCutOff)), innerCutOff(cos(innerCutOff)) {
		float cosTheta = distance / glm::sqrt(distance * distance + baseRadius * baseRadius);
		cosTheta2 = cosTheta * cosTheta;

		direction = normalize(direction);
		float yaw = glm::pi<float>() + std::atan2(direction.x, direction.z);   // around Y
		float pitch = -std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z)); // around X
		rotation = vec3(pitch, yaw, 0);
	}


	void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = true) {
		//b->rotate(vec3(radians(90.0f),0,0));
	//b->translate(vec3(0,0,-1.5));
		mat4 rotationMatrix = glm::rotate(mat4(1), rotation.x, vec3(1, 0, 0));
		glm::rotate(rotationMatrix, rotation.y, vec3(0, 1, 0));

		mat4 correctionMatix = glm::translate(mat4(1), vec3(0, 0, -distance / 2));
		glm::rotate(correctionMatix, radians(90.0f), vec3(1, 0, 0));
		
		mat4 originalMatrix = parentMatrix * getLocalMatrix() * rotationMatrix;
		worldMatrix = originalMatrix * correctionMatix;

		position = vec3(originalMatrix * vec4(0, 0, 0, 1));
		Renderer::lighting.spotLights.push_back(this);
		//Renderer::lighting.ambientLights.push_back(ambientLight);
		//ambientLight->worldMatrix = originalMatrix;
	}


	void bind(Shader* shader, vec3& viewPos) {
		Light::bind(shader, viewPos);

		mat3 directionMatrix = transpose(inverse(mat3(worldMatrix)));
		vec3 direction = normalize(directionMatrix * vec3(0, 0, -1));

		/*
		glm::vec3 D = glm::normalize(direction);
		glm::vec3 v = viewPos - position;
		float t = glm::dot(v, D);
		if (t < 0.0f || t > distance)
			glCullFace(GL_BACK); // outside along axis
		float d2 = glm::dot(v, v);
		float cosAlpha2 = (t * t) / d2;
		if (cosAlpha2 >= cosTheta2)
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);
		*/


		shader->setVec3("light.position", position);
		shader->setVec3("light.direction", direction);
		shader->setFloat("light.distance", distance);
		shader->setFloat("light.cosTheta2", cosTheta2);
		shader->setFloat("light.innerCutoff",innerCutOff);
		shader->setFloat("light.outerCutoff", outerCutOff);

		shader->setMat4("model", worldMatrix);
		shader->setMat3("normalMatrix", directionMatrix);
		draw();
	}

};