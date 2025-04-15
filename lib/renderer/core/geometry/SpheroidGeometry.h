#pragma once
#include "../Geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cmath>

class SpheroidGeometry : public Geometry {
public:
    SpheroidGeometry(double radiusX, double radiusY, double radiusZ, unsigned int segments = 72, unsigned int rings = 72) {
        // Generate vertices, normals, tangents, bitangents, and UVs
        for (unsigned int i = 0; i <= rings; ++i) {
            float v = static_cast<float>(i) / rings;
            float phi = v * glm::pi<float>(); // Polar angle [0, π]

            for (unsigned int j = 0; j <= segments; ++j) {
                float u = static_cast<float>(j) / segments;
                float theta = u * glm::two_pi<float>(); // Azimuthal angle [0, 2π]

                // Position
                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);

                float x = radiusX * sinPhi * cosTheta;
                float y = radiusY * cosPhi;
                float z = radiusZ * sinPhi * sinTheta;

                glm::vec3 position(x, y, z);
                vertices.push_back(position);

                // Normal
                glm::vec3 normal = glm::normalize(position);
                normals.push_back(normal);

                // UV
                uv.push_back(glm::vec2(u, v));

                // Tangent
                glm::vec3 tangent = glm::vec3(
                    -radiusX * sinPhi * sinTheta,
                    0.0f,
                    radiusZ * sinPhi * cosTheta
                );
                if (glm::length(tangent) < 1e-6f)
                    tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                else
                    tangent = glm::normalize(tangent);
                tangents.push_back(tangent);

                // Bitangent
                glm::vec3 bitangent = glm::cross(normal, tangent);
                if (glm::length(bitangent) < 1e-6f)
                    bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
                else
                    bitangent = glm::normalize(bitangent);
                bitangents.push_back(bitangent);
            }
        }

        // Generate indices
        for (unsigned int i = 0; i < rings; ++i) {
            for (unsigned int j = 0; j < segments; ++j) {
                unsigned int first = i * (segments + 1) + j;
                unsigned int second = first + segments + 1;

                // First triangle (CCW winding)
                indices.push_back(first);
                indices.push_back(first + 1);
                indices.push_back(second);

                // Second triangle (CCW winding)
                indices.push_back(first + 1);
                indices.push_back(second + 1);
                indices.push_back(second);
            }
        }

        generateBuffer(); // Generate VBO/VAO after geometry is built
    }

};
