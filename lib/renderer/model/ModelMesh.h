#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../shading/Shader.h"



const int TEXTURE_UNIT_ALBEDO = 0;
const int TEXTURE_UNIT_METALLIC_ROUGHNESS = 1;
const int TEXTURE_UNIT_NORMAL = 2;
const int TEXTURE_UNIT_AMBIENT_OCCLUSION = 3;
const int TEXTURE_UNIT_EMISSIVE = 4;

/**
 * A mesh is a collection of geometry paired with a material.
 */
class ModelMesh {
public:
    struct Vertex {
        glm::vec3 mPosition;
        glm::vec3 mNormal;
        glm::vec2 mTextureCoordinates;
        glm::vec3 mTangent;
        glm::vec3 mBitangent;
    };


    struct Texture {
        unsigned int mId;
        std::string mPath; // used to de-dupe textures loaded
    };


    struct Material {
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

        std::shared_ptr<Texture> textureAlbedo;
        std::shared_ptr<Texture> textureMetallicRoughness;
        std::shared_ptr<Texture> textureNormal;
        std::shared_ptr<Texture> textureAmbientOcclusion;
        std::shared_ptr<Texture> textureEmissive;
    };


    ModelMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) : mVertices(vertices), mIndices(indices), mMaterial(material) {
        init();
    }

    void Draw(Shader* shader) {

        // albedo
        shader->setBool("material.useTextureAlbedo", mMaterial.useTextureAlbedo);
        shader->setVec3("material.albedo", mMaterial.albedo);
        if (mMaterial.useTextureAlbedo) {
            glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_ALBEDO);
            shader->setInt("material.textureAlbedo", TEXTURE_UNIT_ALBEDO);
            glBindTexture(GL_TEXTURE_2D, mMaterial.textureAlbedo->mId);
        }

        shader->setBool("material.useTextureMetallicRoughness", mMaterial.useTextureMetallicRoughness);
        shader->setFloat("material.metallic", mMaterial.metallic);
        shader->setFloat("material.roughness", mMaterial.roughness);
        if (mMaterial.useTextureMetallicRoughness) {
            glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_METALLIC_ROUGHNESS);
            shader->setInt("material.textureMetallicRoughness", TEXTURE_UNIT_METALLIC_ROUGHNESS);
            glBindTexture(GL_TEXTURE_2D, mMaterial.textureMetallicRoughness->mId);
        }

        shader->setBool("material.useTextureNormal", mMaterial.useTextureNormal);
        if (mMaterial.useTextureNormal) {
            glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_NORMAL);
            shader->setInt("material.textureNormal", TEXTURE_UNIT_NORMAL);
            glBindTexture(GL_TEXTURE_2D, mMaterial.textureNormal->mId);
        }

        shader->setBool("material.useTextureAmbientOcclusion", mMaterial.useTextureAmbientOcclusion);
        shader->setFloat("material.ambientOcclusion", mMaterial.ambientOcclusion);
        if (mMaterial.useTextureAmbientOcclusion) {
            glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_AMBIENT_OCCLUSION);
            shader->setInt("material.textureAmbientOcclusion", TEXTURE_UNIT_AMBIENT_OCCLUSION);
            glBindTexture(GL_TEXTURE_2D, mMaterial.textureAmbientOcclusion->mId);
        }

        shader->setBool("material.useTextureEmissive", mMaterial.useTextureEmissive);
        shader->setVec3("material.emissive", mMaterial.emissive);
        if (mMaterial.useTextureEmissive) {
            glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_EMISSIVE);
            shader->setInt("material.textureEmissive", TEXTURE_UNIT_EMISSIVE);
            glBindTexture(GL_TEXTURE_2D, mMaterial.textureEmissive->mId);
        }

        glActiveTexture(GL_TEXTURE0);

        // draw mesh
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

public: 
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	Material mMaterial;

private:
    void init() {
        // create our data structures
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO); // use this VAO for subsequent calls

        glBindBuffer(GL_ARRAY_BUFFER, mVBO); // use this VBO for subsequent calls
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // copy over the vertex data

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO); // use this EBO for subsequent calls
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW); // copy over the index data

        // setup the locations of vertex data
        // positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));

        // texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTextureCoordinates));

        // tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTangent));

        // bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mBitangent));

        glBindVertexArray(0);
    }

private:
    // OpenGL data structures
    unsigned int mVAO, mVBO, mEBO;
};