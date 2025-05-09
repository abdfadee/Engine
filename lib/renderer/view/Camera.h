#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include "../core/Object3d.h";
#include "../shading/Shaders.h"
#include "../shading/UBO.h";
#include "../utility/Controls.h"


using namespace glm;


class Camera : public Object3D {
public:
	UBO cameraBuffer;
	mat4 view, projection;
	vec3 cameraFront , cameraUp;
	double pre_xpos, pre_ypos;
	float sensitivity;
	bool firstMouse = true;


	Camera(float sensitivity = 0.1f) :
		cameraBuffer(UBO(0, sizeof(mat4))),
		sensitivity(sensitivity) {
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
		if (firstMouse) {
			pre_xpos = xpos;
			pre_ypos = ypos;
			firstMouse = false;
		}

		float xoffset = static_cast<float>(xpos - pre_xpos) * sensitivity;
		float yoffset = static_cast<float>(ypos - pre_ypos) * sensitivity;
		pre_xpos = xpos;
		pre_ypos = ypos;

		// Get current orientation
		quat currentOrientation = orientation;

		// Create rotation quaternions for yaw (Y-axis) and pitch (X-axis)
		quat yaw = angleAxis(radians(-xoffset), vec3(0.0f, 1.0f, 0.0f));
		quat pitch = angleAxis(radians(-yoffset), vec3(1.0f, 0.0f, 0.0f));

		// Apply yaw first, then pitch to avoid roll
		quat newOrientation = normalize(yaw * currentOrientation * pitch);

		// Clamp pitch to prevent over-rotation
		//vec3 euler = eulerAngles(newOrientation);
		//const float maxPitch = radians(89.0f);
		//euler.x = glm::clamp(euler.x, -maxPitch, maxPitch);

		// Set new rotation without roll
		setRotation(newOrientation);
	}

	virtual void attachControls() {
		Controls::mouseListener.addListener([&](double xpos, double ypos) {look(xpos, ypos); });
		Controls::addCallback([&](GLFWwindow* window, float delta) {movement(window, delta); });
	}


	void updateViewProjectionMatrix() {
		worldMatrix = getWorldMatrix();

		mat3 directionMatrix = transpose(inverse(mat3(worldMatrix)));
		cameraFront = normalize(directionMatrix * vec3(0, 0, -1));
		cameraUp = normalize(directionMatrix * vec3(0, 1, 0));

		vec3 position = vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));

		view = glm::lookAt(position, position + cameraFront, cameraUp);

		mat4 viewProjectionMatrix = projection * view;
		cameraBuffer.upload(0,sizeof(viewProjectionMatrix),&viewProjectionMatrix);
	}


	void lookAt(vec3 target) {
		worldMatrix = getWorldMatrix();
		vec3 position = vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));
		vec3 direction = normalize(target - position);

		float yaw = glm::pi<float>() + std::atan2(direction.x, direction.z);   // around Y
		float pitch = -std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z)); // around X

		setRotation(vec3(pitch,yaw,0));
	}

};