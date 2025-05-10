#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


enum class ColliderType {
    AABB,
    Sphere,
    Plane
};


class Collider {
public:
    ColliderType type;
    virtual void update(const glm::vec3& position , const glm::quat& orientation) = 0;
};