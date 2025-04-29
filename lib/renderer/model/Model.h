#pragma once
#include "glad/glad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <map>

#include "../core/Mesh.h"
#include "../texture/stbloader.h"
#include "../texture/Texture.h"
#include "../shading/Shader.h"



class Model : public Object3D {
private:
    // data
    std::string directory;
    std::map<std::string, Texture*> texturesLoaded;
    Material* materialOverride;


public:
    Model(std::string path , bool flipTexturesVertically = true) {
        loadModel(path, flipTexturesVertically);
    }


    Model(std::string path, Material* material, bool flipTexturesVertically) : materialOverride(material) {
        loadModel(path, flipTexturesVertically);
    }


private:
    void loadModel(std::string path , bool flipTexturesVertically) {
        Assimp::Importer importer;
        stbi_set_flip_vertically_on_load(flipTexturesVertically);
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        }

        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        stbi_set_flip_vertically_on_load(true);
    }


    // recursively load all meshes in the node tree
    void processNode(aiNode* node, const aiScene* scene) {
        // process all of this node's meshes if it has any
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            add(processMesh(mesh, scene));
        }

        // continue with children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }


	// convert assimp mesh to our own mesh class
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene) {
        Geometry* geometery = new Geometry();

        // vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            // position
            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;

            vertex.mPosition = position;

            // normal
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;

            vertex.mNormal = normal;

            // texture coordinates
            if (mesh->mTextureCoords[0]) {
                glm::vec2 textureCoordinates;
                textureCoordinates.x = mesh->mTextureCoords[0][i].x;
                textureCoordinates.y = mesh->mTextureCoords[0][i].y;
                vertex.mTextureCoordinates = textureCoordinates;
            }
            else {
                vertex.mTextureCoordinates = glm::vec2(0.0f, 0.0f);
            }

            // tangents
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[0].x;
            tangent.y = mesh->mTangents[0].y;
            tangent.z = mesh->mTangents[0].z;
            vertex.mTangent = tangent;

            // bitangents
            glm::vec3 bitangent;
            bitangent.x = mesh->mBitangents[0].x;
            bitangent.y = mesh->mBitangents[0].y;
            bitangent.z = mesh->mBitangents[0].z;
            vertex.mBitangent = bitangent;

            geometery->vertices.push_back(vertex);
        }

        // indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                geometery->indices.push_back(face.mIndices[j]);
            }
        }


        // material
        Material* material = new Material();

        if (materialOverride) {
            material = materialOverride;
            return new Mesh(geometery, material);
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

            // albedo
            if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
                material->useTextureAlbedo = true;
                material->textureAlbedo = loadMaterialTexture(aiMaterial, aiTextureType_DIFFUSE);
            }

            // metallicRoughness (in gltf 2.0 they are combined in one texture)
            if (aiMaterial->GetTextureCount(aiTextureType_UNKNOWN)) {
                material->useTextureMetallicRoughness = true;
                material->textureMetallicRoughness = loadMaterialTexture(aiMaterial, aiTextureType_UNKNOWN);
            }

            // metallic
            if (aiMaterial->GetTextureCount(aiTextureType_METALNESS)) {
                material->useTextureMetallic = true;
                material->textureMetallic = loadMaterialTexture(aiMaterial, aiTextureType_METALNESS);
            }

            // roughness
            if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS)) {
                material->useTextureRoughness = true;
                material->textureRoughness = loadMaterialTexture(aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS);
            }

            // normal
            if (aiMaterial->GetTextureCount(aiTextureType_NORMALS)) {
                material->useTextureNormal = true;
                material->textureNormal = loadMaterialTexture(aiMaterial, aiTextureType_NORMALS);
            }

            // height
            if (aiMaterial->GetTextureCount(aiTextureType_HEIGHT)) {
                material->useTextureHeight = true;
                material->textureHeight = loadMaterialTexture(aiMaterial, aiTextureType_HEIGHT);
            }

            // displacement
            if (aiMaterial->GetTextureCount(aiTextureType_DISPLACEMENT)) {
                material->useTextureHeight = true;
                material->textureHeight = loadMaterialTexture(aiMaterial, aiTextureType_DISPLACEMENT);
            }

            // ambient occlusion
            if (aiMaterial->GetTextureCount(aiTextureType_LIGHTMAP)) {
                material->useTextureAmbientOcclusion = true;
                material->textureAmbientOcclusion = loadMaterialTexture(aiMaterial, aiTextureType_LIGHTMAP);
            }

            // emissive
            if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE)) {
                material->useTextureEmissive = true;
                material->textureEmissive = loadMaterialTexture(aiMaterial, aiTextureType_EMISSIVE);
            }
        }

        return new Mesh(geometery , material);
    }


	// loads the first texture of given type
    Texture* loadMaterialTexture(aiMaterial* material, aiTextureType type) {
        aiString path;
        material->GetTexture(type, 0, &path);

        // check if we already have it loaded and use that if so
        auto iterator = texturesLoaded.find(std::string(path.C_Str()));
        if (iterator != texturesLoaded.end()) {
            return iterator->second;
        }

        std::cout << "Process material: " << path.C_Str() << std::endl;

        std::string fullPath = directory + '/' + path.C_Str();
        bool gamma = false;
        if (type == aiTextureType_DIFFUSE)
            gamma = true;
        Texture* texture = Texture::T_2D(fullPath,gamma);

        // cache it for future lookups
        texturesLoaded.insert(std::pair<std::string, Texture*>(path.C_Str(), texture));

        return texture;
    }
};