#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class RectangularCuboid {
public:
    GLuint vao, indexCount;

    RectangularCuboid() {}

    RectangularCuboid(float width, float height, float depth, int widthSegments = 1, int heightSegments = 1, int depthSegments = 1) {
        glGenVertexArrays(1, &vao);
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions, normals, tangents, bitangents;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        float halfW = width / 2.0f;
        float halfH = height / 2.0f;
        float halfD = depth / 2.0f;

        struct Face {
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            glm::vec3 origin;
            glm::vec3 uDir;
            glm::vec3 vDir;
            int uSegments;
            int vSegments;
        } faces[6] = {
            {{ 0,  0,  1}, { 1, 0,  0}, { 0, 1, 0}, {-halfW, -halfH, halfD}, {width, 0, 0}, {0, height, 0}, widthSegments, heightSegments}, // Front
            {{ 0,  0, -1}, {-1, 0,  0}, { 0, 1, 0}, { halfW, -halfH, -halfD}, {-width, 0, 0}, {0, height, 0}, widthSegments, heightSegments}, // Back
            {{ 1,  0,  0}, { 0, 0, -1}, { 0, 1, 0}, { halfW, -halfH, halfD}, {0, 0, -depth}, {0, height, 0}, depthSegments, heightSegments}, // Right
            {{-1,  0,  0}, { 0, 0,  1}, { 0, 1, 0}, {-halfW, -halfH, -halfD}, {0, 0, depth}, {0, height, 0}, depthSegments, heightSegments}, // Left
            {{ 0,  1,  0}, { 1, 0,  0}, { 0, 0, -1}, {-halfW, halfH, halfD}, {width, 0, 0}, {0, 0, -depth}, widthSegments, depthSegments}, // Top
            {{ 0, -1,  0}, { 1, 0,  0}, { 0, 0,  1}, {-halfW, -halfH, -halfD}, {width, 0, 0}, {0, 0, depth}, widthSegments, depthSegments}  // Bottom
        };

        for (const auto& face : faces) {
            int indexOffset = positions.size();
            for (int v = 0; v <= face.vSegments; ++v) {
                for (int u = 0; u <= face.uSegments; ++u) {
                    glm::vec3 position = face.origin + (face.uDir * (float(u) / face.uSegments)) + (face.vDir * (float(v) / face.vSegments));
                    glm::vec2 texCoord = glm::vec2(float(u) / face.uSegments, float(v) / face.vSegments);
                    positions.push_back(position);
                    normals.push_back(face.normal);
                    uv.push_back(texCoord);
                    tangents.push_back(face.tangent);
                    bitangents.push_back(face.bitangent);
                }
            }

            for (unsigned int v = 0; v < face.vSegments; ++v) {
                for (unsigned int u = 0; u < face.uSegments; ++u) {
                    unsigned int a = indexOffset + (v * (face.uSegments + 1)) + u;
                    unsigned int b = a + 1;
                    unsigned int c = a + (face.uSegments + 1);
                    unsigned int d = c + 1;
                    indices.insert(indices.end(), { a, c, b, b, c, d });
                }
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
