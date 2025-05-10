#pragma once
#include "../Collider.h"

class PlaneCollider : public Collider {
public:
    glm::quat orientation;
    glm::vec3 normal;
    float distance;  // From origin along normal
    float thickness; // Collision thickness (half-extent)

    PlaneCollider(const glm::vec3& normal, float distance, float thickness = 0.1f) :
        normal(glm::normalize(normal)),
        distance(distance),
        thickness(thickness) { type = ColliderType::Plane; }

    void update(const glm::vec3& position, const glm::quat& orientation) {
        this->orientation = orientation;
    }

    glm::vec3 getNormal() const { return glm::normalize(mat3_cast(orientation) * normal); }
    float getDistance() const { return distance; }
    float getThickness() const { return thickness; }

    glm::vec4 getPlaneEquation() const {
        return glm::vec4(getNormal(), distance);
    }

    float signedDistance(const glm::vec3& point) const {
        return glm::dot(getNormal(), point) + distance;
    }
};