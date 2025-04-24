#pragma once
#include "Light.h"


class Lighting {
public:
	vector<Light*> pointLights;
	vector<Light*> rectAreaLights;
	vector<Light*> spotLights;
	vector<Light*> ambientLights;


	void updateDepthMaps (Object3D* root) {
		Shaders::OmnidirectionalDepthMap->use();
		for (int i = 0; i < pointLights.size(); ++i)
			pointLights[i]->updateShadow(root);


		Shaders::DirectionalDepthMap->use();
		for (int i = 0; i < rectAreaLights.size(); ++i)
			rectAreaLights[i]->updateShadow(root);

		for (int i = 0; i < spotLights.size(); ++i)
			spotLights[i]->updateShadow(root);
	}


	void shaderInit(Shader* shader,vec2& pixelSize,vec3 &viewPos) {
		shader->use();
		shader->setVec3("viewPos", viewPos);
		shader->setVec2("pixelSize", pixelSize);
		shader->setInt("gPosition", 0);
		shader->setInt("gNormal", 1);
		shader->setInt("gAlbedo", 2);
		shader->setInt("gMaterial", 3);
	}


	void illuminate(vec3 &viewPos) {
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		vec2 pixelSize = vec2(1.0f / viewport[2], 1.0f / viewport[3]);

		/* Point Lights */
		shaderInit(Shaders::PointLight,pixelSize,viewPos);
		for (int i = 0; i < pointLights.size(); ++i)
			pointLights[i]->bind(Shaders::PointLight,viewPos);

		/* RectArea Lights */
		shaderInit(Shaders::RectAreaLight, pixelSize, viewPos);
		for (int i = 0; i < rectAreaLights.size(); ++i)
			rectAreaLights[i]->bind(Shaders::RectAreaLight, viewPos);

		/* Spot Lights */
		shaderInit(Shaders::SpotLight, pixelSize, viewPos);
		for (int i = 0; i < spotLights.size(); ++i)
			spotLights[i]->bind(Shaders::SpotLight, viewPos);

		/* Ambient Lights */
		shaderInit(Shaders::AmbientLight, pixelSize, viewPos);
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