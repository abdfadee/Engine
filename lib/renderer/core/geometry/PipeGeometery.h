#pragma once
#include "../Geometry.h"


class PipeGeometery : public Geometry {
public:

	PipeGeometery(double innerRadius, double outerRadius, double height, unsigned int segments = 64) {
        const float PI = 3.14159265359f;
        float halfHeight = height * 0.5f;
        
        // Generate vertices for the inner and outer surfaces of the pipe
        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i / (float)segments * 2.0f * PI;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            // Outer surface vertices
            vertices.push_back(glm::vec3(cosTheta * outerRadius, -halfHeight, sinTheta * outerRadius));
            vertices.push_back(glm::vec3(cosTheta * outerRadius, halfHeight, sinTheta * outerRadius));

            // Inner surface vertices
            vertices.push_back(glm::vec3(cosTheta * innerRadius, -halfHeight, sinTheta * innerRadius));
            vertices.push_back(glm::vec3(cosTheta * innerRadius, halfHeight, sinTheta * innerRadius));

            // Normals (pointing outward for outer surface, inward for inner surface)
            glm::vec3 outerNormal = glm::normalize(glm::vec3(cosTheta, 0.0f, sinTheta));
            glm::vec3 innerNormal = -outerNormal;

            normals.push_back(outerNormal);
            normals.push_back(outerNormal);
            normals.push_back(innerNormal);
            normals.push_back(innerNormal);

            // UVs
            uv.push_back(glm::vec2((float)i / (float)segments, 0.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 1.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 0.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 1.0f));

            // Tangents and bitangents
            glm::vec3 tangent = glm::normalize(glm::vec3(-sinTheta, 0.0f, cosTheta));
            glm::vec3 bitangent = glm::vec3(0.0f, 1.0f, 0.0f);

            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        // Generate indices for the outer and inner surfaces
        for (int i = 0; i < segments; ++i) {
            int outerStart = i * 4;
            int innerStart = outerStart + 2;

            // Outer surface
            indices.push_back(outerStart);
            indices.push_back(outerStart + 1);
            indices.push_back(outerStart + 4);

            indices.push_back(outerStart + 1);
            indices.push_back(outerStart + 5);
            indices.push_back(outerStart + 4);

            // Inner surface
            indices.push_back(innerStart);
            indices.push_back(innerStart + 4);
            indices.push_back(innerStart + 1);

            indices.push_back(innerStart + 1);
            indices.push_back(innerStart + 4);
            indices.push_back(innerStart + 5);
        }

        // Generate vertices and indices for the top and bottom caps
        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i / (float)segments * 2.0f * PI;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            // Bottom cap vertices
            vertices.push_back(glm::vec3(cosTheta * outerRadius, -halfHeight, sinTheta * outerRadius));
            vertices.push_back(glm::vec3(cosTheta * innerRadius, -halfHeight, sinTheta * innerRadius));

            // Top cap vertices
            vertices.push_back(glm::vec3(cosTheta * outerRadius, halfHeight, sinTheta * outerRadius));
            vertices.push_back(glm::vec3(cosTheta * innerRadius, halfHeight, sinTheta * innerRadius));

            // Normals for caps
            normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); // Bottom outer
            normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); // Bottom inner
            normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));  // Top outer
            normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));  // Top inner

            // UVs for caps
            uv.push_back(glm::vec2((float)i / (float)segments, 0.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 1.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 0.0f));
            uv.push_back(glm::vec2((float)i / (float)segments, 1.0f));

            // Tangents and bitangents for caps
            glm::vec3 tangent = glm::normalize(glm::vec3(-sinTheta, 0.0f, cosTheta));
            glm::vec3 bitangent = glm::vec3(0.0f, 1.0f, 0.0f);

            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        // Generate indices for the caps
        int capStart = (segments + 1) * 4;
        for (int i = 0; i < segments; ++i) {
            int bottomOuter = capStart + i * 4;
            int bottomInner = bottomOuter + 1;
            int topOuter = bottomOuter + 2;
            int topInner = bottomOuter + 3;

            // Bottom cap
            indices.push_back(bottomOuter);
            indices.push_back(bottomOuter + 4);
            indices.push_back(bottomInner);

            indices.push_back(bottomInner);
            indices.push_back(bottomOuter + 4);
            indices.push_back(bottomInner + 4);

            // Top cap
            indices.push_back(topOuter);
            indices.push_back(topInner);
            indices.push_back(topOuter + 4);

            indices.push_back(topInner);
            indices.push_back(topInner + 4);
            indices.push_back(topOuter + 4);
        }

        generateBuffer();
	}

};