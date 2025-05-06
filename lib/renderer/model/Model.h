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
#include "Node.h"
#include "../animation/Animation.h"



class Model : public Object3D {
public:
    // data
    Node* root;
    std::string directory;
    int m_BoneCounter = 0;
    glm::mat4 m_GlobalInverseTransform;

    std::map<std::string, Texture*> texturesLoaded;
    std::map<string, BoneInfo> m_BoneInfoMap;
    vector<Animation*> animations;
    



    Model(std::string path , bool flipTexturesVertically = true) {
        loadModel(path, flipTexturesVertically);
    }


    void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
        Object3D::render(shader, parentMatrix,materialize,geometeryPass);

        for (auto pair : m_BoneInfoMap)
            shader->setMat4("finalBonesMatrices[" + std::to_string(pair.second.id) + "]", pair.second.finalTransformMatrix);
        
        bool rigged = (m_BoneCounter > 0);
        shader->setBool("rigged",rigged);

        root->render(shader, worldMatrix , materialize, geometeryPass,rigged);

        shader->setBool("rigged", false);
    }


    Node* getNode(string name) {
        Node* result = findByName(root, name);
        if (result == NULL)
            cout << "node not found!" << endl;
        return result;
    }

    Node* findByName(Node* node, string name) {
        if (node->name == name)
            return node;

        for (auto n : node->children) {
            Node* result = findByName(n, name);
            if (result != NULL)
                return result;
        }

        return NULL;
    }

    void GetBoneTransforms()
    {
        glm::mat4 identity = mat4(1.0f);
        ReadNodeHierarchy(root, identity);
    }

private:
    void loadModel(std::string path , bool flipTexturesVertically) {
        Assimp::Importer importer;
        stbi_set_flip_vertically_on_load(flipTexturesVertically);
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs  | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        }

        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        m_GlobalInverseTransform = glm::inverse(AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
        glm::mat4 identitiy = mat4(1);
        root = processNode(scene->mRootNode, scene,identitiy);
        GetBoneTransforms();


        for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
            animations.push_back(new Animation(scene->mAnimations[i],root, m_BoneInfoMap , m_BoneCounter));

        stbi_set_flip_vertically_on_load(true);

        cout << m_BoneInfoMap.size() << endl;
    }


    // recursively load all meshes in the node tree
    Node* processNode(aiNode* node, const aiScene* scene , glm::mat4 &globalTransformation)
    {
        Node* newNode = new Node();

        newNode->name = node->mName.C_Str();

        newNode->transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
        newNode->globalTransformation = globalTransformation * newNode->transformation;

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            newNode->meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            newNode->children.push_back(processNode(node->mChildren[i], scene, newNode->globalTransformation));

        cout << newNode->name << endl;

        return newNode;
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
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.mTangent = tangent;

            // bitangents
            glm::vec3 bitangent;
            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;
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
        Texture* texture = Texture::T_2D(fullPath, gamma);

        // cache it for future lookups
        texturesLoaded.insert(std::pair<std::string, Texture*>(path.C_Str(), texture));

        return texture;
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
        assert("More Bones Required");
    }


    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                m_BoneInfoMap[boneName] = newBoneInfo;
                boneID = m_BoneCounter;
                m_BoneCounter++;
            }
            else
            {
                boneID = m_BoneInfoMap[boneName].id;
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


    void ReadNodeHierarchy(const Node* pNode, const glm::mat4& ParentTransform)
    {
        string NodeName(pNode->name);

        glm::mat4 NodeTransformation(pNode->transformation);
        glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

        if (m_BoneInfoMap.find(NodeName) != m_BoneInfoMap.end()) {
            BoneInfo& boneInfo = m_BoneInfoMap[NodeName];
            uint BoneIndex = boneInfo.id;
            boneInfo.finalTransformMatrix = GlobalTransformation * boneInfo.offset;
        }

        for (uint i = 0; i < pNode->children.size(); i++) {
            ReadNodeHierarchy(pNode->children[i], GlobalTransformation);
        }
    }

};