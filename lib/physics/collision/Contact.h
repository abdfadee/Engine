#pragma once
#include <glm/glm.hpp>
#include "../RigidBody.h"


struct Contact {
    glm::vec3 point;
    glm::vec3 normal;
    float penetration;
    RigidBody* bodyA;
    RigidBody* bodyB;
};