#pragma once
#include "../core/Geometry.h"


class RectangleGeometry : public Geometry {
public:

    RectangleGeometry(double width, double height , unsigned int widthSegments = 1 , unsigned int heightSegments = 1) {
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;

        float segmentWidth = width / widthSegments;
        float segmentHeight = height / heightSegments;

        for (int y = 0; y <= heightSegments; ++y) {
            for (int x = 0; x <= widthSegments; ++x) {
                Vertex vertex;

                float xPos = -halfWidth + x * segmentWidth;
                float yPos = -halfHeight + y * segmentHeight;
                vertex.mPosition = { xPos, yPos, 0.0f };

                // All normals point along the positive Z-axis
                vertex.mNormal = { 0.0f, 0.0f, 1.0f };

                float u = static_cast<float>(x) / widthSegments;
                float v = static_cast<float>(y) / heightSegments;
                vertex.mTextureCoordinates = { u, v };

                vertex.mTangent = { 1.0f, 0.0f, 0.0f };
                vertex.mBitangent = { 0.0f, 1.0f, 0.0f };

                vertices.push_back(vertex);
            }
        }


        for (int y = 0; y < heightSegments; ++y) {
            for (int x = 0; x < widthSegments; ++x) {
                int topLeft = y * (widthSegments + 1) + x;
                int topRight = topLeft + 1;
                int bottomLeft = (y + 1) * (widthSegments + 1) + x;
                int bottomRight = bottomLeft + 1;

                // First triangle
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Second triangle
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

	}


};