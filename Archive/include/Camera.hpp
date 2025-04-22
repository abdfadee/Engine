#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Globals.hpp"
#include "UniformBuffer.hpp";


using namespace glm;


class Camera {
public:
	struct CameraBuffer {
		CameraBuffer(mat4 view, mat4 perspect) : view(view), perspect(perspect) {}
		mat4 view, perspect;
	};

	mat4 view, perspect;
	vec3 cameraPos , cameraFront , cameraUp;
	double pitch, yaw, pre_xpos, pre_ypos;
	float sensetivity , fov , znear , zfar;


	Camera(vec3 cameraPos = vec3(0.0f, 0.0f, 6.0f) , vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f) , double yaw = -90.0, double pitch = 0.0 , float fov = 45.0f , float znear = 0.1f , float zfar = 50.0f , float sensetivity = 0.1f) :
		cameraPos(cameraPos),
		cameraUp(cameraUp),
		yaw(yaw) , pitch(pitch),
		cameraFront(vec3(
			cos(radians(pitch))* cos(radians(yaw)),
			sin(radians(pitch)),
			cos(radians(pitch))* sin(radians(yaw))
		)),
		view(lookAt(cameraPos, cameraPos + cameraFront, cameraUp)),
		fov(fov) , znear(znear) , zfar(zfar),
		perspect(perspective(radians(fov), 1.0f, znear, zfar)),
		sensetivity(sensetivity) {}


	void movement(GLFWwindow* window, float deltaTime) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += deltaTime * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= deltaTime * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= normalize(cross(cameraFront, cameraUp)) * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += normalize(cross(cameraFront, cameraUp)) * deltaTime;

		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
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

		pitch -= yoffset;
		yaw += xoffset;

		if (pitch > 89.0f)
			pitch = 89.0;
		else if (pitch < -89.0f)
			pitch = -89.0;

		pre_xpos = xpos;
		pre_ypos = ypos;

		cameraFront = vec3(
			cos(radians(pitch)) * cos(radians(yaw)),
			sin(radians(pitch)),
			cos(radians(pitch)) * sin(radians(yaw))
		);

		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void zoom(double xoffset, double yoffset) {
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		else if (fov > 45.0f)
			fov = 45.0f;

		perspect = perspective(radians(fov), 1.0f, znear, zfar);
	}

	void update() {
		CameraBuffer buffer{ view,perspect };
		UniformBuffers::cameraBuffer.update(0, sizeof(buffer), &buffer);
	}
};