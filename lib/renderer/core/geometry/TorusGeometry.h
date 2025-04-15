#pragma once
#include "../Geometry.h"
#include <cmath>


class TorusGeometry : public Geometry {
public:

	TorusGeometry(double majorRadius, double minorRadius, unsigned int majorSegments = 32, unsigned int minorSegments = 16) {
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

                vertices.push_back(position);
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

        generateBuffer();
	}

};