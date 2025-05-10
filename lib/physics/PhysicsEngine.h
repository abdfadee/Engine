#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>
#include "RigidBody.h"

class PhysicsEngine {
private:
    std::vector<RigidBody*> bodies;
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);


public:
    void AddBody(RigidBody* body) {
        bodies.push_back(body);
    }

    void SetGravity(const glm::vec3& newGravity) {
        gravity = newGravity;
    }

    void Update(float deltaTime) {
        // Apply forces
        for (auto body : bodies) body->AddForce(gravity * body->mass);

        // Integrate
        for (auto body : bodies) body->Integrate(deltaTime);
    }
};