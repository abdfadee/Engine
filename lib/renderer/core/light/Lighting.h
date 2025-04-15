#pragma once
#include "Light.h"


class Lighting {
public:
	vector<Light*> pointLights;
	vector<Light*> rectAreaLights;
	vector<Light*> spotLights;
	vector<Light*> ambientLights;


	void shaderInit(Shader* shader,vec3 &viewPos,vec2 &pixelSize) {
		shader->use();
		shader->setVec3("viewPos", viewPos);
		shader->setVec2("pixelSize", pixelSize);
		shader->setInt("gNormal", 0);
		shader->setInt("gAlbedo", 1);
		shader->setInt("gMaterial", 2);
		shader->setInt("gDepth", 3);
	}


	void illuminate(vec3 viewPos) {
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		vec2 pixelSize = vec2(1.0f / viewport[2], 1.0f / viewport[3]);

		/* Point Lights */
		shaderInit(Shaders::PointLight,viewPos,pixelSize);
		for (int i = 0; i < pointLights.size(); ++i)
			pointLights[i]->bind(Shaders::PointLight,viewPos);

		/* RectArea Lights */
		shaderInit(Shaders::RectAreaLight, viewPos, pixelSize);
		for (int i = 0; i < rectAreaLights.size(); ++i)
			rectAreaLights[i]->bind(Shaders::RectAreaLight, viewPos);

		/* Spot Lights */
		shaderInit(Shaders::SpotLight, viewPos, pixelSize);
		for (int i = 0; i < spotLights.size(); ++i)
			spotLights[i]->bind(Shaders::SpotLight, viewPos);

		/* Ambient Lights */
		shaderInit(Shaders::AmbientLight, viewPos, pixelSize);
		for (int i = 0; i < ambientLights.size(); ++i)
			ambientLights[i]->bind(Shaders::AmbientLight, viewPos);
	}


	void clear() {
		pointLights.clear();
		rectAreaLights.clear();
		spotLights.clear();
		ambientLights.clear();
	}

};