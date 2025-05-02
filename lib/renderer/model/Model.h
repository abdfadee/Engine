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
#include "../utility/Helpers.h"
#include "BoneInfo.h"



class Model : public Object3D {
public:
    // data
    std::string directory;
    vector<Mesh> meshes;
    std::map<std::string, Texture*> texturesLoaded;
    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    vector<glm::mat4> Transforms;
    glm::mat4 m_GlobalInverseTransform = glm::mat4(1);


    Model(std::string path , bool flipTexturesVertically = true) {
        loadModel(path, flipTexturesVertically);
    }


    void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
        Object3D::render(shader, parentMatrix,materialize,geometeryPass);

        for (int i = 0; i < Transforms.size(); ++i)
            shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", Transforms[i]);

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].render(shader,worldMatrix,materialize,geometeryPass);
    }



    void loadModel(std::string path , bool flipTexturesVertically) {
        Assimp::Importer importer;
        stbi_set_flip_vertically_on_load(flipTexturesVertically);
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        }

        directory = path.substr(0, path.find_last_of('/'));

        m_GlobalInverseTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation);
        m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene, m_GlobalInverseTransform);

        GetBoneTransforms(scene,Transforms);

        stbi_set_flip_vertically_on_load(true);
    }


    // recursively load all meshes in the node tree
    void processNode(aiNode* node, const aiScene* scene , glm::mat4 parentNodeMatrix)
    {
        mat4 transform = parentNodeMatrix * AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh m = processMesh(mesh, scene);
            m.baseMatrix = transform;
            meshes.push_back(m);
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, transform);
        }

    }



	// convert assimp mesh to our own mesh class
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
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

        ExtractBoneWeightForVertices(geometery->vertices, mesh, scene);
        /*
        for (int i = 0; i < geometery->vertices.size(); ++i) {
            cout << endl;

            Vertex vertex = geometery->vertices[i];
            for (int j = 0 ;j < MAX_BONE_INFLUENCE; ++j)
                cout << vertex.m_BoneIDs[j] << ":" << vertex.m_Weights << " , ";

            cout << endl;
        }
        */
        return Mesh(geometery , material);
    }


    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                return;
            }
        }
        //assert("More Bones Required");
    }


    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
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
        Texture* texture = Texture::T_2D(fullPath, gamma);

        // cache it for future lookups
        texturesLoaded.insert(std::pair<std::string, Texture*>(path.C_Str(), texture));

        return texture;
    }


    void GetBoneTransforms(const aiScene* pScene, vector<glm::mat4>& Transforms)
    {
        Transforms.resize(m_BoneInfoMap.size());

        ReadNodeHierarchy(pScene->mRootNode, glm::mat4(1.0f));

        // Iterate over values using range-based for loop
        uint i = 0;
        for (const auto& pair : m_BoneInfoMap) {
            Transforms[i] = pair.second.finalTransformation;
            ++i;
        }
    }

    void ReadNodeHierarchy(const aiNode* pNode, const glm::mat4& ParentTransform)
    {
        string NodeName(pNode->mName.data);

        mat4 NodeTransformation(AssimpGLMHelpers::ConvertMatrixToGLMFormat(pNode->mTransformation));

        mat4 GlobalTransformation = ParentTransform * NodeTransformation;

        if (m_BoneInfoMap.find(NodeName) != m_BoneInfoMap.end()) {
            uint BoneIndex = m_BoneInfoMap[NodeName].id;
            m_BoneInfoMap[NodeName].finalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfoMap[NodeName].offset;
        }

        for (uint i = 0; i < pNode->mNumChildren; i++) {
            ReadNodeHierarchy(pNode->mChildren[i], GlobalTransformation);
        }
    }

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }
};