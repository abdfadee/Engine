#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class Torus {
public:
    GLuint vao, indexCount;

    Torus() {}

    Torus(float majorRadius, float minorRadius, int majorSegments = 32, int minorSegments = 16) {
        glGenVertexArrays(1, &vao);
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions, normals, tangents, bitangents;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        const float PI = 3.14159265359f;

        for (int i = 0; i <= majorSegments; ++i) {
            float theta = i * 2.0f * PI / majorSegments;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            for (int j = 0; j <= minorSegments; ++j) {
                float phi = j * 2.0f * PI / minorSegments;
                float cosPhi = cos(phi);
                float sinPhi = sin(phi);

                glm::vec3 position(
                    (majorRadius + minorRadius * cosPhi) * cosTheta,
                    minorRadius * sinPhi,
                    (majorRadius + minorRadius * cosPhi) * sinTheta
                );

                glm::vec3 normal = glm::normalize(glm::vec3(
                    cosTheta * cosPhi,
                    sinPhi,
                    sinTheta * cosPhi
                ));

                glm::vec3 tangent(-sinTheta, 0.0f, cosTheta);
                glm::vec3 bitangent = glm::cross(normal, tangent);

                positions.push_back(position);
                normals.push_back(normal);
                tangents.push_back(tangent);
                bitangents.push_back(bitangent);
                uv.emplace_back((float)i / majorSegments, (float)j / minorSegments);
            }
        }

        for (unsigned int i = 0; i < majorSegments; ++i) {
            for (unsigned int j = 0; j < minorSegments; ++j) {
                unsigned int a = i * (minorSegments + 1) + j;
                unsigned int b = (i + 1) * (minorSegments + 1) + j;
                unsigned int c = a + 1;
                unsigned int d = b + 1;

                indices.insert(indices.end(), { a, b, c, b, d, c });
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
