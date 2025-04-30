#pragma once
#include <glm/glm.hpp>
#include "../shading/Shader.h"
#include "../texture/Texture.h"



const int TEXTURE_UNIT_ALBEDO = 0;
const int TEXTURE_UNIT_NORMAL = 1;
const int TEXTURE_UNIT_Height = 2;
const int TEXTURE_UNIT_METALLIC_ROUGHNESS = 3;
const int TEXTURE_UNIT_METALLIC = 3;
const int TEXTURE_UNIT_ROUGHNESS = 4;
const int TEXTURE_UNIT_AMBIENT_OCCLUSION = 5;
const int TEXTURE_UNIT_EMISSIVE = 6;



class Material {
public:
    bool useTextureAlbedo = false;
    bool useTextureNormal = false;
    bool useTextureHeight = false;
    bool useTextureMetallicRoughness = false;
    bool useTextureMetallic = false;
    bool useTextureRoughness = false;
    bool useTextureAmbientOcclusion = false;
    bool useTextureEmissive = false;

    vec2 uvMultiplier = vec2(1);
    glm::vec3 albedo = glm::vec3(1.0, 0, 0);
    float heightScale = 0.1f;
    float metallic = 0.0f;
    float roughness = 0.0f;
    float ambientOcclusion = 1.0f;
    glm::vec3 emissive = glm::vec3(0.0, 0.0, 0.0);

    Texture* textureAlbedo;
    Texture* textureNormal;
    Texture* textureHeight;
    Texture* textureMetallicRoughness;
    Texture* textureMetallic;
    Texture* textureRoughness;
    Texture* textureAmbientOcclusion;
    Texture* textureEmissive;


    Material() {}


    Material(glm::vec3 albedo,float metallic,float roughness,float ambientOcclusion = 1.0f,glm::vec3 emissive = vec3(0)) {}


    Material(Texture* textureAlbedo,Texture* textureNormal,Texture* textureHeight,Texture* textureMetallic,Texture* textureRoughness,Texture* textureAmbientOcclusion = NULL,Texture* textureEmissive = NULL) {
        if (textureAlbedo != NULL) {
            useTextureAlbedo = true;
            this->textureAlbedo = textureAlbedo;
        }

        if (textureNormal != NULL) {
            useTextureNormal = true;
            this->textureNormal = textureNormal;
        }

        if (textureHeight != NULL) {
            useTextureHeight = true;
            this->textureHeight = textureHeight;
        }

        if (textureMetallic != NULL) {
            useTextureMetallic = true;
            this->textureMetallic = textureMetallic;
        }

        if (textureRoughness != NULL) {
            useTextureRoughness = true;
            this->textureRoughness = textureRoughness;
        }

        if (textureAmbientOcclusion != NULL) {
            useTextureAmbientOcclusion = true;
            this->textureAmbientOcclusion = textureAmbientOcclusion;
        }

        if (textureEmissive != NULL) {
            useTextureEmissive = true;
            this->textureEmissive = textureEmissive;
        }
    }



    void bind(Shader* shader) {
        // Albedo
        shader->setVec3("material.albedo", albedo);
        shader->setBool("material.useTextureAlbedo", useTextureAlbedo);
        if (useTextureAlbedo)
            textureAlbedo->bind(TEXTURE_UNIT_ALBEDO);
        else
            Texture::deactivateTextureUnit(TEXTURE_UNIT_ALBEDO);



        // Material
        shader->setFloat("material.metallic", metallic);
        shader->setFloat("material.roughness", roughness);

        shader->setBool("material.useTextureMetallicRoughness", useTextureMetallicRoughness);
        shader->setBool("material.useTextureMetallic", useTextureMetallic);
        shader->setBool("material.useTextureRoughness", useTextureRoughness);

        if (useTextureMetallicRoughness)
            textureMetallicRoughness->bind(TEXTURE_UNIT_METALLIC_ROUGHNESS);
        else {
            Texture::deactivateTextureUnit(TEXTURE_UNIT_METALLIC_ROUGHNESS);

            if (useTextureMetallic)
                textureMetallic->bind(TEXTURE_UNIT_METALLIC);
            else
                Texture::deactivateTextureUnit(TEXTURE_UNIT_METALLIC);
            if (useTextureRoughness)
                textureRoughness->bind(TEXTURE_UNIT_ROUGHNESS);
            else
                Texture::deactivateTextureUnit(TEXTURE_UNIT_ROUGHNESS);
        }
        
        

        // Normal
        shader->setBool("material.useTextureNormal", useTextureNormal);
        if (useTextureNormal)
            textureNormal->bind(TEXTURE_UNIT_NORMAL);
        else
            Texture::deactivateTextureUnit(TEXTURE_UNIT_NORMAL);



        // Height
        shader->setBool("material.useTextureHeight", useTextureHeight);
        if (useTextureHeight) {
            textureHeight->bind(TEXTURE_UNIT_Height);
            shader->setFloat("material.heightScale", heightScale);
        }    
        else
            Texture::deactivateTextureUnit(TEXTURE_UNIT_Height);



        // AO
        shader->setFloat("material.ambientOcclusion", ambientOcclusion);
        shader->setBool("material.useTextureAmbientOcclusion", useTextureAmbientOcclusion);
        if (useTextureAmbientOcclusion)
            textureAmbientOcclusion->bind(TEXTURE_UNIT_AMBIENT_OCCLUSION);
        else
            Texture::deactivateTextureUnit(TEXTURE_UNIT_AMBIENT_OCCLUSION);



        // Emissive
        shader->setVec3("material.emissive", emissive);
        shader->setBool("material.useTextureEmissive", useTextureEmissive);
        if (useTextureEmissive)
            textureEmissive->bind(TEXTURE_UNIT_EMISSIVE);
        else
            Texture::deactivateTextureUnit(TEXTURE_UNIT_EMISSIVE);



        shader->setVec2("uvMultiplier", uvMultiplier);
    }

};