#pragma once
#include "glad/glad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <map>
#include "../core/Vertex.h"
#include "../core/Mesh.h"
#include "../texture/stbloader.h"
#include "../texture/Texture.h"


#include "../shading/Shader.h"

/**
 * A collection of meshes.
 */
class Model {
public:
    /**
     * Load a glTF 2.0 model.
     * @param path
     */
    Model(std::string path , bool flipTexturesVertically = true) {
        loadModel(path, flipTexturesVertically);
    }


    /**
     * Load a glTF 2.0 model using a provided material. This will ignore any material
     * present in the model file.
     * @param path
     */
    Model(std::string path, Material* material, bool flipTexturesVertically) : mMaterialOverride(material) {
        loadModel(path, flipTexturesVertically);
    }



    void Draw(Shader* shader) {
        for (auto& mesh : mMeshes) {
            mesh.Draw(shader);
        }
    }


private:
    void loadModel(std::string path, bool flipTexturesVertically) {
        Assimp::Importer importer;
        stbi_set_flip_vertically_on_load(flipTexturesVertically);
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        }

        mDirectory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        stbi_set_flip_vertically_on_load(true);
    }

    // recursively load all meshes in the node tree
    void processNode(aiNode* node, const aiScene* scene) {
        // process all of this node's meshes if it has any
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            mMeshes.push_back(processMesh(mesh, scene));
        }

        // continue with children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

	// convert assimp mesh to our own mesh class
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        if (mMaterialOverride) {
            material = *mMaterialOverride;
        }

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

            vertices.push_back(vertex);
        }

        // indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        Geometry geometery = Geometry(vertices,indices);


        // material
        if (!mMaterialOverride) {
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

                // albedo
                if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
                    material.useTextureAlbedo = true;
                    material.textureAlbedo = loadMaterialTexture(aiMaterial, aiTextureType_DIFFUSE);
                }

                // metallicRoughness (in gltf 2.0 they are combined in one texture)
                if (aiMaterial->GetTextureCount(aiTextureType_UNKNOWN)) {
                    // defined here in assimp https://github.com/assimp/assimp/blob/master/include/assimp/pbrmaterial.h#L57
                    material.useTextureMetallicRoughness = true;
                    material.textureMetallicRoughness = loadMaterialTexture(aiMaterial, aiTextureType_UNKNOWN);
                }

                // normal
                if (aiMaterial->GetTextureCount(aiTextureType_NORMALS)) {
                    material.useTextureNormal = true;
                    material.textureNormal = loadMaterialTexture(aiMaterial, aiTextureType_NORMALS);
                }

                // ambient occlusion
                if (aiMaterial->GetTextureCount(aiTextureType_LIGHTMAP)) {
                    material.useTextureAmbientOcclusion = true;
                    material.textureAmbientOcclusion = loadMaterialTexture(aiMaterial, aiTextureType_LIGHTMAP);
                }

                // emissive
                if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE)) {
                    material.useTextureEmissive = true;
                    material.textureEmissive = loadMaterialTexture(aiMaterial, aiTextureType_EMISSIVE);
                }
            }
        }

        return ModelMesh(geometery , material);
    }

	// loads the first texture of given type
    Texture* loadMaterialTexture(aiMaterial* material, aiTextureType type) {
        aiString path;
        material->GetTexture(type, 0, &path);

        // check if we already have it loaded and use that if so
        auto iterator = mTexturesLoaded.find(std::string(path.C_Str()));
        if (iterator != mTexturesLoaded.end()) {
            return iterator->second;
        }

        std::cout << "Process material: " << path.C_Str() << std::endl;

        std::string fullPath = mDirectory + '/' + path.C_Str();
        bool gamma = false;
        if (type == aiTextureType_DIFFUSE)
            gamma = true;
        Texture* texture = Texture::T_2D(fullPath,gamma);

        // cache it for future lookups
        mTexturesLoaded.insert(std::pair<std::string, Texture*>(path.C_Str(), texture));

        return texture;
    }


private:
    // data
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
    std::map<std::string, Texture*> mTexturesLoaded;
    std::shared_ptr<Material> mMaterialOverride;
};