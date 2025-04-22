#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/BoxGeometry.h"
#include "../../utility/helpers.h"


class RectAreaLight : public Light {
	float width , height , distance , halfSizes[3];
	vec3 rotation , position;

public:
	RectAreaLight(vec3 color, float intensity , vec3 direction , float width , float height , float distance) :
		width(width), height(height), distance(distance),
		Light(color, intensity , new BoxGeometry(width,height,distance) ) {
		halfSizes[0] = width / 2; halfSizes[1] = height / 2; halfSizes[2] = distance / 2;
		direction = normalize(direction);
		float yaw = glm::pi<float>() + std::atan2(direction.x, direction.z);   // around Y
		float pitch = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z)); // around X
		rotation = vec3(pitch, yaw, 0);
	}


	void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = true) {
		mat4 rotationMatrix = glm::rotate(mat4(1), rotation.x, vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, vec3(0, 1, 0));

		mat4 correctionMatrix = glm::translate(mat4(1), vec3(0, 0, -distance / 2));

		mat4 originalMatrix = parentMatrix * getLocalMatrix() * rotationMatrix;
		worldMatrix = originalMatrix * correctionMatrix;

		position = vec3(originalMatrix * vec4(0, 0, 0, 1));
		Renderer::lighting.rectAreaLights.push_back(this);
	}


	void bind(Shader* shader, vec3 &viewPos) {
		Light::bind(shader, viewPos);

		vec3 center = getWorldPosition();

		mat3 directionMatrix = transpose(inverse(mat3(worldMatrix)));
		vec3 axes[3];
		axes[0] = normalize(directionMatrix * vec3(1, 0, 0));
		axes[1] = normalize(directionMatrix * vec3(0, 1, 0));
		axes[2] = normalize(directionMatrix * vec3(0, 0, -1));
		cout << axes[2] << endl;

		vec3 d = viewPos - center;
		glCullFace(GL_FRONT);
		for (int i = 0; i < 3; ++i) {
			float dist = dot(d, axes[i]);
			if (std::abs(dist) > halfSizes[i] + 0.1) {
				glCullFace(GL_BACK);
				break;
			}
		}

		shader->setVec3("light.position", position);
		shader->setVec3("light.center", center);
		string str = "light.axes[";
		for (int i = 0; i < 3; ++i)
			shader->setVec3(str + to_string(i) + "]",axes[i]);
		str = "light.halfSizes[";
		for (int i = 0; i < 3; ++i)
			shader->setFloat(str + to_string(i) + "]", halfSizes[i]);

		shader->setMat4("model", worldMatrix);
		shader->setMat3("normalMatrix", directionMatrix);
		draw();
	}


};