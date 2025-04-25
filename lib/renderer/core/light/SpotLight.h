#pragma once
#include "Light.h"
#include "../shading/Shader.h"
#include "../Renderer.h"
#include "../geometry/ConeGeometry.h"
#include "../../utility/helpers.h"
#include "../shadow/DirectionalShadow.h"


class SpotLight : public Light{
	public:
	DirectionalShadow* shadow;
	mat3 directionMatrix;
	float innerCutOff, outerCutOff, distance , cosTheta2;
	vec3 rotation, position , direction , up;


	SpotLight(vec3 color, float intensity, float oc, float ic, float distance) :
		distance(distance), outerCutOff(cos(oc)), innerCutOff(cos(ic)),
		Light(color, intensity, new ConeGeometry(distance * tan(oc), distance))
	{
		float baseRadius = distance * tan(oc);
		float cosTheta = cos(atan(baseRadius / distance));
		cosTheta2 = cosTheta * cosTheta;

		shadow = new DirectionalShadow(oc*2 ,distance);
	}


	void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
		if (!geometeryPass) return;
		mat4 correctionMatix = glm::translate(mat4(1), vec3(0, 0, -distance));
		correctionMatix = glm::rotate(correctionMatix, radians(90.0f), vec3(1, 0, 0));
		
		mat4 originalMatrix = parentMatrix * getLocalMatrix();
		worldMatrix = originalMatrix * correctionMatix;

		position = vec3(originalMatrix * vec4(0, 0, 0, 1));
		directionMatrix = transpose(inverse(mat3(worldMatrix)));
		direction = normalize(directionMatrix * vec3(0, -1, 0));
		up = normalize(directionMatrix * vec3(0, 0, -1));

		cout << direction << endl;

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

		shader->setMat4("light.viewProjectionMatrix", shadow->viewProjectionMatrix);
		shader->setInt("depthMap", 4);
		shader->setFloat("bias",shadow->bias);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());

		draw();
	}


	void updateShadow(Object3D* root) {
		shadow->updateDepthMap(position, direction, up, root);
	}

};