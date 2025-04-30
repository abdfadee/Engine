#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/BoxGeometry.h"
#include "../utility/helpers.h"
#include "../shadow/DirectionalShadow.h"


class RectAreaLight : public Light {
public:
	DirectionalShadow* shadow;
	mat3 directionMatrix;
	vec3 position, axes[3];
	float width , height , distance , halfSizes[3];
	bool unified;

	RectAreaLight(vec3 color, float intensity , float width , float height , float distance , bool unified = true) :
		unified(unified),
		width(width), height(height), distance(distance),
		Light(color, intensity , new BoxGeometry(width,height,distance) ) {
		halfSizes[0] = width / 2; halfSizes[1] = height / 2; halfSizes[2] = distance / 2;
		shadow = new DirectionalShadow(width, height, distance);
	}


	void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
		if (!geometeryPass) return;

		mat4 correctionMatrix = glm::translate(mat4(1), vec3(0, 0, -distance / 2));

		worldMatrix = parentMatrix * getLocalMatrix();
		position = vec3(worldMatrix * vec4(0, 0, 0, 1));

		worldMatrix = worldMatrix * correctionMatrix;
		directionMatrix = transpose(inverse(mat3(worldMatrix)));
		axes[0] = normalize(directionMatrix * vec3(1, 0, 0));
		axes[1] = normalize(directionMatrix * vec3(0, 1, 0));
		axes[2] = normalize(directionMatrix * vec3(0, 0, -1));

		Renderer::lights.push_back(this);
	}


	void bind(Shader* shader, vec3 &viewPos) {
		Light::bind(shader, viewPos);

		vec3 center = getWorldPosition();

		vec3 d = viewPos - center;
		glCullFace(GL_FRONT);
		for (int i = 0; i < 3; ++i) {
			float dist = dot(d, axes[i]);
			if (std::abs(dist) > halfSizes[i] + 0.1) {
				glCullFace(GL_BACK);
				break;
			}
		}

		shader->setInt("light.type", 2);
		shader->setBool("light.unified", unified);
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

		shader->setMat4("light.viewProjectionMatrix", shadow->viewProjectionMatrix);
		shader->setFloat("bias", shadow->bias);
		shader->setInt("depthMap", TEXTURE_UNIT_DEPTH);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_DEPTH);
		glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());

		draw();
	}


	void updateShadow(Object3D* root) {
		Shaders::DirectionalDepthMap->use();
		shadow->updateDepthMap(position, axes[2], axes[1], root);
	}


};