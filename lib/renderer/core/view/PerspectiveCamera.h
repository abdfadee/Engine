#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"


using namespace glm;


class PerspectiveCamera : public Camera {
public:
	float fov, znear, zfar;

	PerspectiveCamera(float fov = 45.0f, float znear = 0.1f, float zfar = 50.0f, float sensetivity = 0.1f) :
		Camera(sensetivity),
		fov(fov),
		znear(znear),
		zfar(zfar) {
		projection = perspective(radians(fov), 1.0f, znear, zfar);
	}


	void zoom(double xoffset, double yoffset) {
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		else if (fov > 45.0f)
			fov = 45.0f;

		projection = perspective(radians(fov), 1.0f, znear, zfar);
	}

	void attachControls() {
		Camera::attachControls();
		Controls::scrollListener.addListener([&](double xoffset, double yoffset) {zoom(xoffset, yoffset); });
	}

};