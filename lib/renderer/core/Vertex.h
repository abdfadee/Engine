#pragma once
#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4


struct Vertex {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTextureCoordinates;
    glm::vec3 mTangent;
    glm::vec3 mBitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];


    Vertex() {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            m_BoneIDs[i] = -1;
            m_Weights[i] = 0.0f;
        }
    }

};