#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class EllipticalCylinder {
public:
    GLuint vao, indexCount;

    EllipticalCylinder() {}

    EllipticalCylinder(float radiusX, float radiusZ, float height, int radialSegments = 32, int heightSegments = 1) {
        glGenVertexArrays(1, &vao);
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions, normals, tangents, bitangents;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        const float PI = 3.14159265359f;

        for (int y = 0; y <= heightSegments; ++y) {
            float v = float(y) / heightSegments;
            float currentHeight = v * height - height / 2.0f;

            for (int i = 0; i <= radialSegments; ++i) {
                float angle = (float(i) / radialSegments) * 2.0f * PI;
                float cosA = cos(angle);
                float sinA = sin(angle);

                glm::vec3 position = glm::vec3(radiusX * cosA, currentHeight, radiusZ * sinA);
                glm::vec3 normal = glm::normalize(glm::vec3(cosA / radiusX, 0.0f, sinA / radiusZ));
                glm::vec3 tangent = glm::normalize(glm::vec3(-sinA, 0.0f, cosA));
                glm::vec3 bitangent = glm::cross(normal, tangent);

                positions.push_back(position);
                normals.push_back(normal);
                tangents.push_back(tangent);
                bitangents.push_back(bitangent);
                uv.push_back(glm::vec2(float(i) / radialSegments, v));
            }
        }

        for (unsigned int y = 0; y < heightSegments; ++y) {
            for (unsigned int i = 0; i < radialSegments; ++i) {
                unsigned int a = y * (radialSegments + 1) + i;
                unsigned int b = a + 1;
                unsigned int c = a + (radialSegments + 1);
                unsigned int d = c + 1;

                indices.insert(indices.end(), { a, c, b, b, c, d });
            }
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
