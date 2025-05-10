#pragma once
#include "../Collider.h"

class SphereCollider : public Collider {
public:
    glm::vec3 position;
    float radius;

    SphereCollider (const glm::vec3& position, float radius) : position(position), radius(radius) {
        type = ColliderType::Sphere;
    }

    void update(const glm::vec3& position, const glm::quat& orientation) { this->position = position; };

    glm::vec3 getCenter() const { return position; }
    float getRadius() const { return radius; }
};