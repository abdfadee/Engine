#pragma once
#include "Light.h"
#include "../framebuffer/FrameBuffer.h"
#include "IBL.h"


class Lighting {
public:
	vector<Light*> pointLights;
	vector<Light*> rectAreaLights;
	vector<Light*> spotLights;
	vector<Light*> ambientLights;
	IBL* ibl;


	Lighting(GLuint width, GLuint height) : ibl(new IBL()) {}


	void updateDepthMaps (Object3D* root) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f, 4.0f); // tweak these

		Shaders::OmnidirectionalDepthMap->use();
		for (int i = 0; i < pointLights.size(); ++i)
			pointLights[i]->updateShadow(root);


		Shaders::DirectionalDepthMap->use();
		for (int i = 0; i < rectAreaLights.size(); ++i)
			rectAreaLights[i]->updateShadow(root);

		for (int i = 0; i < spotLights.size(); ++i)
			spotLights[i]->updateShadow(root);

		glDisable(GL_POLYGON_OFFSET_FILL);
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

		glDisable(GL_CULL_FACE);
		shaderInit(Shaders::IBL, pixelSize, viewPos);
		Shaders::IBL->setInt("irradianceMap", 5);
		Shaders::IBL->setInt("prefilterMap", 6);
		Shaders::IBL->setInt("brdfLUT", 7);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->maps[1]);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->maps[2]);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, ibl->maps[3]);
		Shaders::screen->render(Shaders::IBL);
	}


	void clear() {
		pointLights.clear();
		rectAreaLights.clear();
		spotLights.clear();
		ambientLights.clear();
	}

};