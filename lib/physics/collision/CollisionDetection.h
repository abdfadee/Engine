#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>
#include "Contact.h"
#include "Collider.h"
#include "colliders/SphereCollider.h"
#include "colliders/AABBCollider.h"
#include "colliders/PlaneCollider.h"



class CollisionDetection {
public:
    void detectCollisions(std::vector<RigidBody*>& bodies) {
        contacts.clear();

        // Broad phase (naive approach - for small numbers of bodies)
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                if (!bodies[i]->collider || !bodies[j]->collider) continue;

                // Check if AABBs overlap first (if both are AABBs)
                if (bodies[i]->collider->type == ColliderType::AABB &&
                    bodies[j]->collider->type == ColliderType::AABB) {
                    AABBxAABBCollision(bodies[i], bodies[j]);
                }
                // Add other collision type checks here...
            }
        }
    }


    void handleCollisions(float dt) {
        for (auto& contact : contacts) {
            RigidBody* bodyA = contact.bodyA;
            RigidBody* bodyB = contact.bodyB;

            // Calculate relative velocity
            glm::vec3 rA = contact.point - bodyA->position;
            glm::vec3 rB = contact.point - bodyB->position;

            glm::vec3 velA = bodyA->velocity + glm::cross(bodyA->angularVelocity, rA);
            glm::vec3 velB = bodyB->velocity + glm::cross(bodyB->angularVelocity, rB);
            glm::vec3 relativeVel = velA - velB;

            // Calculate impulse
            float contactVel = glm::dot(relativeVel, contact.normal);
            if (contactVel > 0) continue; // Moving apart

            float restitution = glm::min(bodyA->restitution, bodyB->restitution);
            float j = -(1.0f + restitution) * contactVel;

            // Apply impulse
            glm::vec3 impulse = contact.normal * j;
            bodyA->velocity += impulse * bodyA->inverseMass;
            bodyB->velocity -= impulse * bodyB->inverseMass;

            // Apply positional correction to prevent sinking
            const float k_slop = 0.01f; // Penetration allowance
            const float percent = 0.2f; // Penetration percentage to correct
            glm::vec3 correction = contact.normal *
                (glm::max(contact.penetration - k_slop, 0.0f) /
                    (bodyA->inverseMass + bodyB->inverseMass)) * percent;

            bodyA->position += correction * bodyA->inverseMass;
            bodyB->position -= correction * bodyB->inverseMass;
        }
    }



private:
	std::vector<Contact> contacts;

    


    void AABBxAABBCollision(RigidBody* bodyA, RigidBody* bodyB) {
        if (bodyA->collider->type != ColliderType::AABB ||
            bodyB->collider->type != ColliderType::AABB) return;

        AABBCollider* a = static_cast<AABBCollider*>(bodyA->collider);
        AABBCollider* b = static_cast<AABBCollider*>(bodyB->collider);

        glm::vec3 minA = a->getMinExtent();
        glm::vec3 maxA = a->getMaxExtent();
        glm::vec3 minB = b->getMinExtent();
        glm::vec3 maxB = b->getMaxExtent();

        // Check for overlap on all axes
        if (maxA.x > minB.x && minA.x < maxB.x &&
            maxA.y > minB.y && minA.y < maxB.y &&
            maxA.z > minB.z && minA.z < maxB.z) {

            // Calculate penetration and contact normal
            glm::vec3 distances1 = maxB - minA;
            glm::vec3 distances2 = maxA - minB;
            glm::vec3 penetration = glm::min(distances1, distances2);

            float minPenetration = glm::min(glm::min(penetration.x, penetration.y), penetration.z);
            glm::vec3 normal(0.0f);

            if (minPenetration == penetration.x) {
                normal = glm::vec3(maxB.x < maxA.x ? 1.0f : -1.0f, 0.0f, 0.0f);
            }
            else if (minPenetration == penetration.y) {
                normal = glm::vec3(0.0f, maxB.y < maxA.y ? 1.0f : -1.0f, 0.0f);
            }
            else {
                normal = glm::vec3(0.0f, 0.0f, maxB.z < maxA.z ? 1.0f : -1.0f);
            }

            // Create contact
            Contact contact;
            contact.point = (minA + maxA + minB + maxB) * 0.25f; // Approximate contact point
            contact.normal = normal;
            contact.penetration = minPenetration;
            contact.bodyA = bodyA;
            contact.bodyB = bodyB;

            contacts.push_back(contact);
        }
    }


};