#pragma once
#include <glad/glad.h>
#include "shading/Shader.h"
#include "texture/Texture.h"

 
class Material {
	vec3 color = vec3(0.0f);
	float roughness = 0.0f;
	float metallic = 0.0f;
	float heightScale = 0.0f;
	float ao = 0.0f;
	Texture* textureMaps[6] = {nullptr,nullptr, nullptr, nullptr, nullptr, nullptr};


public: 
	Material(Texture* albedoMap , Texture* roughnessMap, Texture* metallicMap, Texture* normalMap = nullptr, Texture* displacmentMap = nullptr, float heightScale = 1.0f ,Texture* aoMap = nullptr) {
		textureMaps[0] = albedoMap;
		textureMaps[1] = roughnessMap;
		textureMaps[2] = metallicMap;
		textureMaps[3] = normalMap;
		textureMaps[4] = displacmentMap;
		textureMaps[5] = aoMap;
		this->heightScale = (displacmentMap == nullptr) ? 0.0f : heightScale;
	}


	Material(vec3 color , float roughness , float metallic , float ao = 0.0f) :
		color(color),
		roughness(roughness),
		metallic(metallic),
		ao(ao) {}


	void bind(Shader *shader) {
		shader->setVec3("color",color);
		shader->setFloat("roughness",roughness);
		shader->setFloat("metallic", metallic);
		shader->setFloat("ambientOcclusion",ao);
		shader->setFloat("heightScale", heightScale);

		for (int i = 0; i < 6; ++i) {
			if (textureMaps[i] != nullptr)
				textureMaps[i]->bind(shader,i);
			else {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

	}

};