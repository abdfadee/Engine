#pragma once
#include "../Collider.h"

class AABBCollider : public Collider {
public:
    glm::vec3 position;
    glm::vec3 halfExtents;

    AABBCollider (glm::vec3 halfExtents) : halfExtents(halfExtents) {
        type = ColliderType::AABB;
    }

    void update(const glm::vec3& position, const glm::quat& orientation) { this->position = position; };

    glm::vec3 getMinExtent() { return position - halfExtents; }
    glm::vec3 getMaxExtent() { return position + halfExtents; }
};