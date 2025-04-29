#pragma once
#include "../core/Geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cmath>

class ConeGeometry : public Geometry {
public:
    ConeGeometry(double baseRadius, double height, unsigned int segments = 72) {
        // Side vertices (around base perimeter)
        for (unsigned int i = 0; i <= segments; ++i) {
            Vertex vertex;

            float u = static_cast<float>(i) / segments;
            float theta = u * glm::two_pi<float>();

            float x = baseRadius * std::cos(theta);
            float y = 0.0f;
            float z = baseRadius * std::sin(theta);

            glm::vec3 position(x, y, z);
            vertex.mPosition = position;

            // Normal (approximate for cone side)
            glm::vec3 normal = glm::normalize(glm::vec3(x, baseRadius / height, z));
            vertex.mNormal = normal;

            // UV (u along circumference, v at base)
            vertex.mTextureCoordinates = glm::vec2(u, 0.0f);

            // Tangent (around the base)
            glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));

            // Bitangent
            glm::vec3 bitangent = glm::cross(normal, tangent);
            
            vertex.mTangent = tangent;
            vertex.mBitangent = bitangent;

            vertices.push_back(vertex);
        }

        // Apex vertex (top point)
        Vertex apex;
        apex.mPosition = glm::vec3(0.0f, height, 0.0f);
        apex.mNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        apex.mTextureCoordinates = glm::vec2(0.5f, 1.0f);
        apex.mTangent = glm::vec3(1.0f, 0.0f, 0.0f);
        apex.mBitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(apex);


        unsigned int apexIndex = static_cast<unsigned int>(vertices.size() - 1);

        // Side triangles (fixed CCW winding)
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(i);
            indices.push_back(apexIndex);
            indices.push_back((i + 1) % (segments + 1));
        }

        // Center vertex for base
        Vertex baseCenter;
        baseCenter.mPosition = glm::vec3 (0.0f, 0.0f, 0.0f);
        baseCenter.mNormal = glm::vec3(0.0f, -1.0f, 0.0f);
        baseCenter.mTextureCoordinates = glm::vec2(0.5f, 0.5f);
        baseCenter.mTangent = glm::vec3(1.0f, 0.0f, 0.0f);
        baseCenter.mBitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(baseCenter);


        unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size() - 1);

        // Base triangles (fixed CCW winding when viewed from below)
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(baseCenterIndex);
            indices.push_back(i);
            indices.push_back((i + 1) % (segments + 1));
        }

    }
};
