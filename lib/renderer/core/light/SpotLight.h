#pragma once
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/ConeGeometry.h"
#include "../../utility/helpers.h"
#include "../shadow/DirectionalShadowMapping.h"


class SpotLight : public Light{
	public:
	DirectionalShadowMapping* shadowMap;
	mat3 directionMatrix;
	float innerCutOff, outerCutOff, distance , cosTheta2;
	vec3 rotation, position , direction , up;


	SpotLight(vec3 color, float intensity, vec3 direction, float oc, float ic, float distance) :
		distance(distance), outerCutOff(cos(oc)), innerCutOff(cos(ic)),
		Light(color, intensity, new ConeGeometry(distance * tan(oc), distance))
	{
		float baseRadius = distance * tan(oc);
		float cosTheta = cos(atan(baseRadius / distance));
		cosTheta2 = cosTheta * cosTheta;

		direction = normalize(direction);
		float yaw = glm::pi<float>() + std::atan2(direction.x, direction.z);   // around Y
		float pitch = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z)); // around X
		rotation = vec3(pitch, yaw, 0);

		shadowMap = new DirectionalShadowMapping(800, 800, oc ,distance);
	}


	void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
		if (!geometeryPass) return;
		
		mat4 rotationMatrix = glm::rotate(mat4(1), rotation.x, vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, vec3(0, 1, 0));

		mat4 correctionMatix = glm::translate(mat4(1), vec3(0, 0, -distance));
		correctionMatix = glm::rotate(correctionMatix, radians(90.0f), vec3(1, 0, 0));
		
		mat4 originalMatrix = parentMatrix * getLocalMatrix() * rotationMatrix;
		worldMatrix = originalMatrix * correctionMatix;

		position = vec3(originalMatrix * vec4(0, 0, 0, 1));
		directionMatrix = transpose(inverse(mat3(worldMatrix)));
		direction = normalize(directionMatrix * vec3(0, -1, 0));
		up = normalize(directionMatrix * vec3(0, 0, -1));

		Renderer::lighting.spotLights.push_back(this);
	}


	void bind(Shader* shader, vec3& viewPos) {
		Light::bind(shader, viewPos);

		glm::vec3 v = viewPos - position;
		float t = glm::dot(v, direction);
		if (t < 0.0f || t > distance) {
			glCullFace(GL_BACK); // outside along axis
		}
		
		else {
			float d2 = glm::dot(v, v);
			float cosAlpha2 = (t * t) / d2;
			if (cosAlpha2 >= cosTheta2)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);
		}

		
		shader->setVec3("light.position", position);
		shader->setVec3("light.direction", direction);
		shader->setFloat("light.distance", distance);
		shader->setFloat("light.cosTheta2", cosTheta2);
		shader->setFloat("light.innerCutOff",innerCutOff);
		shader->setFloat("light.outerCutOff", outerCutOff);

		shader->setMat4("model", worldMatrix);
		draw();
	}


	void updateShadowMap(Object3D* root) {
		shadowMap->updateDepthMap(position, direction, up, root);
	}

};