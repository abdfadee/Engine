#pragma once

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class Pipe {
public:
    GLuint pipeVAO, indexCount;

    Pipe() {}

    Pipe(float innerRadius, float outerRadius, float height, int segments = 64) {
        glGenVertexArrays(1, &pipeVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions, normals, tangents, bitangents;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        const float PI = 3.14159265359f;
        float halfHeight = height * 0.5f;

        // Generate vertices for the inner and outer surfaces of the pipe
        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i / (float)segments * 2.0f * PI;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            // Outer surface vertices
            positions.push_back(glm::vec3(cosTheta * outerRadius, -halfHeight, sinTheta * outerRadius));
            positions.push_back(glm::vec3(cosTheta * outerRadius, halfHeight, sinTheta * outerRadius));

            // Inner surface vertices
            positions.push_back(glm::vec3(cosTheta * innerRadius, -halfHeight, sinTheta * innerRadius));
            positions.push_back(glm::vec3(cosTheta * innerRadius, halfHeight, sinTheta * innerRadius));

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
            positions.push_back(glm::vec3(cosTheta * outerRadius, -halfHeight, sinTheta * outerRadius));
            positions.push_back(glm::vec3(cosTheta * innerRadius, -halfHeight, sinTheta * innerRadius));

            // Top cap vertices
            positions.push_back(glm::vec3(cosTheta * outerRadius, halfHeight, sinTheta * outerRadius));
            positions.push_back(glm::vec3(cosTheta * innerRadius, halfHeight, sinTheta * innerRadius));

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

        indexCount = static_cast<unsigned int>(indices.size());

        // Pack data into a single buffer
        std::vector<float> data;
        for (size_t i = 0; i < positions.size(); ++i) {
            data.insert(data.end(), { positions[i].x, positions[i].y, positions[i].z });
            data.insert(data.end(), { normals[i].x, normals[i].y, normals[i].z });
            data.insert(data.end(), { uv[i].x, uv[i].y });
            data.insert(data.end(), { tangents[i].x, tangents[i].y, tangents[i].z });
            data.insert(data.end(), { bitangents[i].x, bitangents[i].y, bitangents[i].z });
        }

        // Upload to GPU
        glBindVertexArray(pipeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Vertex attributes
        unsigned int stride = (3 + 3 + 2 + 3 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
    }

    void draw() {
        glBindVertexArray(pipeVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};