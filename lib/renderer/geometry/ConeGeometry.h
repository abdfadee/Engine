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
            float u = static_cast<float>(i) / segments;
            float theta = u * glm::two_pi<float>();

            float x = baseRadius * std::cos(theta);
            float y = 0.0f;
            float z = baseRadius * std::sin(theta);

            glm::vec3 position(x, y, z);
            vertices.push_back(position);

            // Normal (approximate for cone side)
            glm::vec3 normal = glm::normalize(glm::vec3(x, baseRadius / height, z));
            normals.push_back(normal);

            // UV (u along circumference, v at base)
            uv.push_back(glm::vec2(u, 0.0f));

            // Tangent (around the base)
            glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));
            tangents.push_back(tangent);

            // Bitangent
            glm::vec3 bitangent = glm::cross(normal, tangent);
            bitangents.push_back(glm::normalize(bitangent));
        }

        // Apex vertex (top point)
        glm::vec3 apex(0.0f, height, 0.0f);
        vertices.push_back(apex);
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        uv.push_back(glm::vec2(0.5f, 1.0f));
        tangents.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        bitangents.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int apexIndex = static_cast<unsigned int>(vertices.size() - 1);

        // Side triangles (fixed CCW winding)
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(i);
            indices.push_back(apexIndex);
            indices.push_back((i + 1) % (segments + 1));
        }

        // Center vertex for base
        glm::vec3 baseCenter(0.0f, 0.0f, 0.0f);
        vertices.push_back(baseCenter);
        normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        uv.push_back(glm::vec2(0.5f, 0.5f));
        tangents.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        bitangents.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size() - 1);

        // Base triangles (fixed CCW winding when viewed from below)
        for (unsigned int i = 0; i < segments; ++i) {
            indices.push_back(baseCenterIndex);
            indices.push_back(i);
            indices.push_back((i + 1) % (segments + 1));
        }

        generateBuffer(); // Generate VBO/VAO after geometry is built
    }
};
