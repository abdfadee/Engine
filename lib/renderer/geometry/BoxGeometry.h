#pragma once
#include "../core/Geometry.h"
#include <glm/glm.hpp>


class BoxGeometry : public Geometry {
public:
    BoxGeometry(double width, double height, double depth,
        int widthSegments = 1, int heightSegments = 1, int depthSegments = 1) {
        // Front (+Z)
        createPlane({ 0, 0, 1 }, { 1, 0, 0 }, { 0, 1, 0 }, width, height, depth, widthSegments, heightSegments);

        // Back (-Z)
        createPlane({ 0, 0, -1 }, { -1, 0, 0 }, { 0, 1, 0 }, width, height, depth, widthSegments, heightSegments);

        // Top (+Y)
        createPlane({ 0, 1, 0 }, { 1, 0, 0 }, { 0, 0, -1 }, width, depth, height, widthSegments, depthSegments);

        // Bottom (-Y)
        createPlane({ 0, -1, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, width, depth, height, widthSegments, depthSegments);

        // Right (+X)
        createPlane({ 1, 0, 0 }, { 0, 0, -1 }, { 0, 1, 0 }, depth, height, width, depthSegments, heightSegments);

        // Left (-X)
        createPlane({ -1, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 }, depth, height, width, depthSegments, heightSegments);

        generateBuffer();
    }

private:

    void createPlane(glm::vec3 normal, glm::vec3 right, glm::vec3 up,
        float width, float height, float depth,
        int widthSegments, int heightSegments) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        float segmentWidth = width / widthSegments;
        float segmentHeight = height / heightSegments;

        int startIndex = vertices.size();

        for (int y = 0; y <= heightSegments; ++y) {
            for (int x = 0; x <= widthSegments; ++x) {
                glm::vec3 position = (right * (-halfWidth + x * segmentWidth)) + (up * (-halfHeight + y * segmentHeight)) + (normal * depth * 0.5f);
                vertices.push_back(position);
                normals.push_back(normal);
                glm::vec2 uvCoord = glm::vec2((float)x / widthSegments, (float)y / heightSegments);
                uv.push_back(uvCoord);

                glm::vec3 tangent = glm::normalize(right);
                glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));
                tangents.push_back(tangent);
                bitangents.push_back(bitangent);
            }
        }

        for (int y = 0; y < heightSegments; ++y) {
            for (int x = 0; x < widthSegments; ++x) {
                int a = startIndex + y * (widthSegments + 1) + x;
                int b = startIndex + y * (widthSegments + 1) + (x + 1);
                int c = startIndex + (y + 1) * (widthSegments + 1) + (x + 1);
                int d = startIndex + (y + 1) * (widthSegments + 1) + x;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(d);
            }
        }
    }
};
