#pragma once
#include <glm/glm.hpp>
#include "../shading/Shader.h"
#include "../texture/Texture.h"


const int TEXTURE_UNIT_ALBEDO = 0;
const int TEXTURE_UNIT_METALLIC_ROUGHNESS = 1;
const int TEXTURE_UNIT_NORMAL = 2;
const int TEXTURE_UNIT_AMBIENT_OCCLUSION = 3;
const int TEXTURE_UNIT_EMISSIVE = 4;


struct GLTFMaterial {
    bool useTextureAlbedo = false;
    bool useTextureMetallicRoughness = false;
    bool useTextureNormal = false;
    bool useTextureAmbientOcclusion = false;
    bool useTextureEmissive = false;

    glm::vec3 albedo = glm::vec3(1.0, 0, 0);
    float metallic = 1.0f;
    float roughness = 0.0f;
    float ambientOcclusion = 1.0f;
    glm::vec3 emissive = glm::vec3(0.0, 0.0, 0.0);

    Texture* textureAlbedo;
    Texture* textureMetallicRoughness;
    Texture* textureNormal;
    Texture* textureAmbientOcclusion;
    Texture* textureEmissive;


    void bind(Shader* shader) {
        // albedo
        shader->setBool("material.useTextureAlbedo", useTextureAlbedo);
        shader->setVec3("material.albedo", albedo);
        if (useTextureAlbedo)
            textureAlbedo->bind(TEXTURE_UNIT_ALBEDO);

        shader->setBool("material.useTextureMetallicRoughness", useTextureMetallicRoughness);
        shader->setFloat("material.metallic", metallic);
        shader->setFloat("material.roughness", roughness);
        if (useTextureMetallicRoughness)
            textureAlbedo->bind(TEXTURE_UNIT_METALLIC_ROUGHNESS);

        shader->setBool("material.useTextureNormal", useTextureNormal);
        if (useTextureNormal)
            textureAlbedo->bind(TEXTURE_UNIT_NORMAL);

        shader->setBool("material.useTextureAmbientOcclusion", useTextureAmbientOcclusion);
        shader->setFloat("material.ambientOcclusion", ambientOcclusion);
        if (useTextureAmbientOcclusion)
            textureAlbedo->bind(TEXTURE_UNIT_AMBIENT_OCCLUSION);

        shader->setBool("material.useTextureEmissive", useTextureEmissive);
        shader->setVec3("material.emissive", emissive);
        if (useTextureEmissive)
            textureAlbedo->bind(TEXTURE_UNIT_EMISSIVE);
    }

};