#pragma once
#include <glm/glm.hpp>

class AABBCollider {
public:
    glm::vec3 position;
    glm::vec3 halfExtents;

    AABBCollider (glm::vec3 halfExtents) : halfExtents(halfExtents) {}

    void update(const glm::vec3& position) { this->position = position; };

    glm::vec3 getMinExtent() { return position - halfExtents; }
    glm::vec3 getMaxExtent() { return position + halfExtents; }
};