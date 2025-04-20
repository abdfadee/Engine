#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include "../3d/Object3d.h";
#include "../shading/shaders/Shaders.h"
#include "../shading/UBO.h";
#include "../../utility/Controls.h"


using namespace glm;


class Camera : public Object3D {
public:
	UBO cameraBuffer;
	mat4 view, projection;
	vec3 cameraFront , cameraUp;
	double pre_xpos, pre_ypos;
	float sensetivity;


	Camera(float sensetivity = 0.1f) :
		cameraBuffer(UBO(0, 32 * sizeof(float))),
		sensetivity(sensetivity) {
		view = glm::lookAt(positionVector, positionVector + cameraFront, cameraUp);
	}


	void movement(GLFWwindow* window, float deltaTime) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			translate(deltaTime * cameraFront);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			translate(-deltaTime * cameraFront);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			translate(-normalize(cross(cameraFront, cameraUp)) * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			translate(normalize(cross(cameraFront, cameraUp)) * deltaTime);
	}

	void look(double xpos, double ypos) {
		static bool firstTime = true;
		if (firstTime) {
			pre_xpos = xpos;
			pre_ypos = ypos;
			firstTime = false;
		}

		double xoffset = (xpos - pre_xpos) * sensetivity;
		double yoffset = (ypos - pre_ypos) * sensetivity;

		rotate(vec3(radians(-yoffset), 0, 0));
		rotate(vec3(0,radians(-xoffset), 0));

		constexpr float maxR = radians(89.0f);
		constexpr float minR = radians(-89.0f);

		if (rotationVector.x > maxR)
			setRotation(vec3(maxR,rotationVector.y,rotationVector.z));
		else if (rotationVector.x < minR)
			setRotation(vec3(minR, rotationVector.y, rotationVector.z));

		pre_xpos = xpos;
		pre_ypos = ypos;
	}

	virtual void attachControls() {
		Controls::mouseListener.addListener([&](double xpos, double ypos) {look(xpos, ypos); });
		Controls::addCallback([&](GLFWwindow* window, float delta) {movement(window, delta); });
	}


	mat4 getViewProjectionMatrix() {
		worldMatrix = getWorldMatrix();

		mat3 directionMatrix = transpose(inverse(mat3(worldMatrix)));
		cameraFront = normalize(directionMatrix * vec3(0, 0, -1));
		cameraUp = normalize(directionMatrix * vec3(0, 1, 0));

		vec3 position = vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));

		view = glm::lookAt(position, position + cameraFront, cameraUp);

		return projection * view;
	}


	void lookAt(vec3 target) {
		worldMatrix = getWorldMatrix();
		vec3 position = vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));
		vec3 direction = normalize(target - position);

		float yaw = glm::pi<float>() + std::atan2(direction.x, direction.z);   // around Y
		float pitch = -std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z)); // around X

		setRotation(vec3(pitch,yaw,0));
	}


	struct CameraBuffer {
		mat4 view, projection;
	};

};