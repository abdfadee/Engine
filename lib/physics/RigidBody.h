#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "collision/AABBCollider.h"



class RigidBody {
public:
    Object3D* representation;

    glm::vec3 position;
    glm::quat orientation;

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 angularVelocity = glm::vec3(0.0f);

    float mass = 1.0f;
    float inverseMass;
    float restitution = 0.5f;  // Bounciness
    float friction = 0.4f;      // Surface friction

    glm::vec3 forceAccum = glm::vec3(0.0f);
    glm::vec3 torqueAccum = glm::vec3(0.0f);

    AABBCollider collider = AABBCollider(vec3(0));

    RigidBody(Object3D* o , float m = 1.0f,float rest = 0.5f,float frict = 0.4f) :
        representation(o),
        position(o->getWorldPosition()), orientation(o->getWorldOrientation()),
        mass(m), restitution(rest), friction(frict) {
        if (mass != 0.0f) inverseMass = 1.0f / mass;
    }

    void setAABBCollider(glm::vec3 halfExtents) {
        this->collider = AABBCollider(halfExtents);
        collider.update(position);
    }

    void AddForce(const glm::vec3& force) {
        if (mass != 0.0f) {
            forceAccum += force;
        }
    }

    void AddForce(const glm::vec3& force, const glm::vec3& point) {
        if (mass != 0.0f) {
            forceAccum += force;
            torqueAccum += glm::cross(point - position, force);
        }
    }

    void ClearForces() {
        forceAccum = glm::vec3(0.0f);
        torqueAccum = glm::vec3(0.0f);
    }

    void Integrate(float deltaTime) {
        if (mass == 0.0f || deltaTime <= 0.0f) return;

        // Linear integration
        glm::vec3 acceleration = forceAccum * inverseMass;
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

        // Angular integration (simplified)
        if (glm::length(angularVelocity) > 0.0f) {
            glm::quat angularChange(
                0.0f,
                angularVelocity.x * 0.5f * deltaTime,
                angularVelocity.y * 0.5f * deltaTime,
                angularVelocity.z * 0.5f * deltaTime
            );
            orientation += angularChange * orientation;
            orientation = glm::normalize(orientation);
        }

        collider.update(position);

        representation->setPosition(position);
        representation->setRotation(orientation);

        ClearForces();
    }
};