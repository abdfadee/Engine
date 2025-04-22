#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Cone {
public:
    GLuint vao, indexCount;

    Cone() {}

    Cone(float radius, float height, int radialSegments = 32) {
        glGenVertexArrays(1, &vao);
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions, normals, tangents, bitangents;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        const float PI = 3.14159265359f;

        // Generate base vertices
        positions.emplace_back(0.0f, -height / 2.0f, 0.0f);
        normals.emplace_back(0.0f, -1.0f, 0.0f);
        uv.emplace_back(0.5f, 0.5f);
        tangents.emplace_back(1.0f, 0.0f, 0.0f);
        bitangents.emplace_back(0.0f, 0.0f, 1.0f);

        for (int i = 0; i <= radialSegments; ++i) {
            float angle = (float(i) / radialSegments) * 2.0f * PI;
            float cosA = cos(angle);
            float sinA = sin(angle);

            positions.emplace_back(radius * cosA, -height / 2.0f, radius * sinA);
            normals.emplace_back(0.0f, -1.0f, 0.0f);
            uv.emplace_back((cosA + 1.0f) / 2.0f, (sinA + 1.0f) / 2.0f);
            tangents.emplace_back(-sinA, 0.0f, cosA);
            bitangents.emplace_back(0.0f, 1.0f, 0.0f);
        }

        // Tip vertex
        int tipIndex = positions.size();
        positions.emplace_back(0.0f, height / 2.0f, 0.0f);
        normals.emplace_back(0.0f, 1.0f, 0.0f);
        uv.emplace_back(0.5f, 1.0f);
        tangents.emplace_back(1.0f, 0.0f, 0.0f);
        bitangents.emplace_back(0.0f, 0.0f, 1.0f);

        // Generate indices for base
        for (int i = 1; i <= radialSegments; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Generate indices for sides
        for (int i = 1; i <= radialSegments; ++i) {
            indices.push_back(tipIndex);
            indices.push_back(i + 1);
            indices.push_back(i);
        }

        indexCount = indices.size();

        std::vector<float> data;
        for (size_t i = 0; i < positions.size(); ++i) {
            data.insert(data.end(), { positions[i].x, positions[i].y, positions[i].z });
            data.insert(data.end(), { normals[i].x, normals[i].y, normals[i].z });
            data.insert(data.end(), { uv[i].x, uv[i].y });
            data.insert(data.end(), { tangents[i].x, tangents[i].y, tangents[i].z });
            data.insert(data.end(), { bitangents[i].x, bitangents[i].y, bitangents[i].z });
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};
