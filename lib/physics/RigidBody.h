#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "RigidBodyState.h"


class RigidBody : public RigidBodyState {
public:
    glm::vec3 force;            // Accumulated force
    glm::vec3 torque;           // Accumulated torque
    float boundingRadius;       // For simple collision detection

    RigidBody(float mass = 1.0f, const glm::mat3& inertia = glm::mat3(1.0f), float radius = 1.0f)
        : RigidBodyState(mass, inertia),
        force(0.0f),
        torque(0.0f),
        boundingRadius(radius)
    {}

    // Apply a force at a world-space point
    void applyForce(const glm::vec3& force, const glm::vec3& point) {
        this->force += force;

        // Calculate torque: τ = r × F
        glm::vec3 r = point - position;
        torque += glm::cross(r, force);
    }

    // Apply an impulse at a world-space point (inherited from RigidBodyState)
    void applyImpulse(const glm::vec3& impulse, const glm::vec3& point) override {
        linearMomentum += impulse;

        // Calculate angular impulse: ΔL = r × J
        glm::vec3 r = point - position;
        angularMomentum += glm::cross(r, impulse);

        recalculate();
    }
};