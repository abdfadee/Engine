#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


class RigidBodyState {
public:
    // Primary state variables
    glm::vec3 position;          // World-space position (m)
    glm::vec3 linearMomentum;    // Linear momentum (kg·m/s)
    glm::quat orientation;       // Rotation from body to world space
    glm::vec3 angularMomentum;   // Angular momentum (kg·m²/s)

    // Secondary state variables (calculated from primary)
    glm::vec3 velocity;          // Linear velocity (m/s)
    glm::vec3 angularVelocity;   // Angular velocity (rad/s)
    glm::quat spin;              // Orientation time derivative
    glm::mat4 worldTransform;    // Body to world transform matrix
    glm::mat4 invWorldTransform; // World to body transform matrix

    // Constant properties
    float mass;                 // Mass (kg)
    float inverseMass;          // 1/mass for faster calculations
    glm::mat3 inertiaTensor;    // Moment of inertia tensor (kg·m²)
    glm::mat3 invInertiaTensor; // Inverse inertia tensor

    // Constructor
    RigidBodyState(float mass = 1.0f, const glm::mat3& inertia = glm::mat3(1.0f))
        : position(0.0f),
        linearMomentum(0.0f),
        orientation(1.0f, 0.0f, 0.0f, 0.0f),
        angularMomentum(0.0f),
        mass(mass),
        inertiaTensor(inertia)
    {
        // Precompute inverses
        inverseMass = mass > 0.0f ? 1.0f / mass : 0.0f;
        invInertiaTensor = glm::inverse(inertiaTensor);

        // Initialize secondary values
        recalculate();
    }

    // Recalculate all secondary values from primary state
    void recalculate() {
        // Linear motion
        velocity = linearMomentum * inverseMass;

        // Angular motion
        angularVelocity = invInertiaTensor * angularMomentum;

        // Orientation spin (dq/dt = 0.5 * ω * q)
        glm::quat angVelQuat(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        spin = 0.5f * angVelQuat * orientation;

        // Normalize orientation to prevent drift
        orientation = glm::normalize(orientation);

        // Update transformation matrices
        updateTransforms();
    }

    // Apply impulse at a world-space point (for collisions)
    void applyImpulse(const glm::vec3& impulse, const glm::vec3& worldPoint) {
        // Linear component
        linearMomentum += impulse;

        // Angular component (torque = r × impulse)
        glm::vec3 r = worldPoint - position;
        angularMomentum += glm::cross(r, impulse);

        recalculate();
    }

    // Get velocity at a specific world-space point
    glm::vec3 getPointVelocity(const glm::vec3& worldPoint) const {
        glm::vec3 r = worldPoint - position;
        return velocity + glm::cross(angularVelocity, r);
    }

    // Convert world-space vector to body-space
    glm::vec3 worldToBody(const glm::vec3& worldVec) const {
        return glm::vec3(invWorldTransform * glm::vec4(worldVec, 0.0f));
    }

    // Convert body-space vector to world-space
    glm::vec3 bodyToWorld(const glm::vec3& bodyVec) const {
        return glm::vec3(worldTransform * glm::vec4(bodyVec, 0.0f));
    }

private:
    // Update transformation matrices from position and orientation
    void updateTransforms() {
        // Create rotation matrix from quaternion
        glm::mat4 rotation = glm::toMat4(orientation);

        // Create translation matrix
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

        // Combined transform (first rotate, then translate)
        worldTransform = translation * rotation;

        // Inverse transform (for converting world to body coordinates)
        invWorldTransform = glm::inverse(worldTransform);
    }
};