#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "RigidBody.h"



// Contact information structure
struct Contact {
    glm::vec3 point;           // World-space contact point
    glm::vec3 normal;          // Contact normal (points toward second body)
    float penetration;         // Penetration depth
    RigidBody* bodyA;          // First body in contact
    RigidBody* bodyB;          // Second body in contact (nullptr for static)
    glm::vec3 localPointA;     // Contact point in bodyA's space
    glm::vec3 localPointB;     // Contact point in bodyB's space
};

// Physics engine class
class PhysicsEngine {
public:
    PhysicsEngine() : gravity(0.0f, -9.81f, 0.0f), damping(0.998f) {}

    // Add a rigid body to the simulation
    void addBody(std::shared_ptr<RigidBody> body) {
        bodies.push_back(body);
    }

    // Set gravity acceleration
    void setGravity(const glm::vec3& g) { gravity = g; }

    // Main simulation step
    void step(float dt) {
        // Apply forces
        applyForces();

        // Detect collisions
        detectCollisions();

        // Solve constraints
        solveConstraints(dt);

        // Integrate motion
        integrate(dt);
    }

    // Get all bodies in the simulation
    const std::vector<std::shared_ptr<RigidBody>>& getBodies() const { return bodies; }

private:
    std::vector<std::shared_ptr<RigidBody>> bodies;
    std::vector<Contact> contacts;
    glm::vec3 gravity;
    float damping; // Global velocity damping factor

    // Apply forces to all bodies
    void applyForces() {
        for (auto& body : bodies) {
            if (body->inverseMass > 0.0f) {
                // Apply gravity
                body->force += gravity / body->inverseMass;

                // Apply damping
                body->linearMomentum *= damping;
                body->angularMomentum *= damping;
            }
        }
    }

    // Basic collision detection (sphere-sphere for demo)
    void detectCollisions() {
        contacts.clear();

        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                auto& bodyA = bodies[i];
                auto& bodyB = bodies[j];

                // Simple sphere-sphere collision
                glm::vec3 delta = bodyB->position - bodyA->position;
                float distance = glm::length(delta);
                float minDistance = bodyA->boundingRadius + bodyB->boundingRadius;

                if (distance < minDistance) {
                    Contact contact;
                    contact.bodyA = bodyA.get();
                    contact.bodyB = bodyB.get();
                    contact.normal = glm::normalize(delta);
                    contact.penetration = minDistance - distance;
                    contact.point = bodyA->position + contact.normal * bodyA->boundingRadius;

                    // Store local contact points
                    contact.localPointA = bodyA->worldToBody(contact.point - bodyA->position);
                    contact.localPointB = bodyB->worldToBody(contact.point - bodyB->position);

                    contacts.push_back(contact);
                }
            }
        }
    }

    // Solve constraints and contacts
    void solveConstraints(float dt) {
        const int iterations = 10;
        for (int i = 0; i < iterations; ++i) {
            for (auto& contact : contacts) {
                resolveContact(contact, dt);
            }
        }
    }

    // Resolve a single contact
    void resolveContact(Contact& contact, float dt) {
        RigidBody* bodyA = contact.bodyA;
        RigidBody* bodyB = contact.bodyB;

        // Calculate relative velocity at contact point
        glm::vec3 velA = bodyA->getPointVelocity(contact.point);
        glm::vec3 velB = bodyB ? bodyB->getPointVelocity(contact.point) : glm::vec3(0.0f);
        glm::vec3 relativeVel = velA - velB;

        // Calculate velocity along contact normal
        float velAlongNormal = glm::dot(relativeVel, contact.normal);

        // Don't resolve if objects are separating
        if (velAlongNormal > 0) return;

        // Calculate restitution (bounciness)
        float restitution = 0.5f; // Could be material property

        // Calculate impulse scalar
        float invMassA = bodyA->inverseMass;
        float invMassB = bodyB ? bodyB->inverseMass : 0.0f;

        // Calculate impulse due to rotation
        glm::vec3 rA = contact.point - bodyA->position;
        glm::vec3 rB = bodyB ? contact.point - bodyB->position : glm::vec3(0.0f);

        glm::vec3 invInertiaA = bodyA->invInertiaTensor * glm::cross(rA, contact.normal);
        glm::vec3 invInertiaB = bodyB ? bodyB->invInertiaTensor * glm::cross(rB, contact.normal) : glm::vec3(0.0f);

        float invRotationalEffect = glm::dot(glm::cross(invInertiaA, rA) +
            glm::dot(glm::cross(invInertiaB, rB), contact.normal);

        float j = -(1.0f + restitution) * velAlongNormal;
        j /= (invMassA + invMassB) + invRotationalEffect;

        // Apply impulse
        glm::vec3 impulse = contact.normal * j;
        bodyA->applyImpulse(-impulse, contact.point);
        if (bodyB) bodyB->applyImpulse(impulse, contact.point);

        // Apply positional correction to prevent sinking
        const float k_slop = 0.01f; // Penetration allowance
        const float percent = 0.2f;  // Percentage to correct

        float correction = (std::max(contact.penetration - k_slop, 0.0f) / (invMassA + invMassB) * percent;
        glm::vec3 correctionVec = contact.normal * correction;

        bodyA->position -= correctionVec * invMassA;
        if (bodyB) bodyB->position += correctionVec * invMassB;
    }

    // Integrate motion using semi-implicit Euler
    void integrate(float dt) {
        for (auto& body : bodies) {
            if (body->inverseMass > 0.0f) {
                // Integrate linear motion
                body->linearMomentum += body->force * dt;
                body->velocity = body->linearMomentum * body->inverseMass;
                body->position += body->velocity * dt;

                // Integrate angular motion
                body->angularMomentum += body->torque * dt;
                body->angularVelocity = body->invInertiaTensor * body->angularMomentum;

                // Integrate orientation
                glm::quat angVelQuat(0.0f,
                    body->angularVelocity.x,
                    body->angularVelocity.y,
                    body->angularVelocity.z);
                body->spin = 0.5f * angVelQuat * body->orientation;
                body->orientation += body->spin * dt;
                body->orientation = glm::normalize(body->orientation);

                // Clear forces and torques
                body->force = glm::vec3(0.0f);
                body->torque = glm::vec3(0.0f);

                // Update world transforms
                body->updateTransforms();
            }
        }
    }
};