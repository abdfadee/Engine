#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "RigidBody.h"
#include "BoxCollider.h"



class PhysicsEngine {
private:
    std::vector<RigidBody*> bodies;
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

    void ResolveCollision(RigidBody& bodyA, RigidBody& bodyB,
        const BoxCollider::CollisionInfo& collision) {
        if (!collision.isColliding) return;

        // Calculate relative velocity
        glm::vec3 rv = bodyB.velocity - bodyA.velocity;
        float velAlongNormal = glm::dot(rv, collision.normal);

        // Do not resolve if objects are separating
        if (velAlongNormal > 0) return;

        // Calculate restitution (bounciness)
        float e = std::min(bodyA.restitution, bodyB.restitution);

        // Calculate impulse scalar
        float j = -(1.0f + e) * velAlongNormal;
        j /= bodyA.inverseMass + bodyB.inverseMass;

        // Apply impulse
        glm::vec3 impulse = j * collision.normal;
        bodyA.velocity -= impulse * bodyA.inverseMass;
        bodyB.velocity += impulse * bodyB.inverseMass;

        // Friction (tangential impulse)
        glm::vec3 tangent = rv - collision.normal * velAlongNormal;
        if (glm::length(tangent) > 0.001f) {
            tangent = glm::normalize(tangent);

            // Friction impulse magnitude
            float jt = -glm::dot(rv, tangent);
            jt /= bodyA.inverseMass + bodyB.inverseMass;

            // Coulomb's law (dynamic/static friction)
            float mu = (bodyA.friction + bodyB.friction) * 0.5f;
            glm::vec3 frictionImpulse;
            if (std::abs(jt) < j * mu) {
                frictionImpulse = jt * tangent; // Dynamic friction
            }
            else {
                frictionImpulse = -j * tangent * mu; // Static friction
            }

            // Apply friction
            bodyA.velocity -= frictionImpulse * bodyA.inverseMass;
            bodyB.velocity += frictionImpulse * bodyB.inverseMass;
        }

        // Positional correction (prevents sinking)
        const float percent = 0.2f;
        const float slop = 0.01f;
        glm::vec3 correction = std::max(collision.penetrationDepth - slop, 0.0f) /
            (bodyA.inverseMass + bodyB.inverseMass) *
            percent * collision.normal;

        bodyA.position -= correction * bodyA.inverseMass;
        bodyB.position += correction * bodyB.inverseMass;
    }

public:
    ~PhysicsEngine() {
        for (auto body : bodies) {
            delete body;
        }
    }

    void AddBody(RigidBody* body) {
        bodies.push_back(body);
    }

    void SetGravity(const glm::vec3& newGravity) {
        gravity = newGravity;
    }

    void Update(float deltaTime) {
        // Apply forces
        for (auto body : bodies) {
            body->AddForce(gravity * body->mass);
        }

        // Check collisions and resolve
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                if (bodies[i]->collider && bodies[j]->collider) {
                    auto collision = bodies[i]->collider->GetCollisionInfo(*bodies[j]->collider);
                    if (collision.isColliding) {
                        ResolveCollision(*bodies[i], *bodies[j], collision);
                    }
                }
            }
        }

        // Integrate all bodies
        for (auto body : bodies) {
            body->Integrate(deltaTime);
        }
    }
};