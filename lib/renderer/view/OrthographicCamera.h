#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"


using namespace glm;


class OrthographicCamera : public Camera {
public:

	OrthographicCamera(float left = -1.0f, float right = 1.0f, float bottom = -1.0f, float top = 1.0f, float znear = 0.1f, float zfar = 50.0f, float sensetivity = 0.1f) :
		Camera(sensetivity) {
		projection = ortho(left,right,bottom,top,znear,zfar);
	}

};