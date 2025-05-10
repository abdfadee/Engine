#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>
#include "Contact.h"
#include "Collider.h"


class CollisionDetection {
public:
	void detectCollisions(std::vector<RigidBody*>& bodies) {

	}

	void handleCollisions() {
		for (auto contact : contacts)
			handleCollision(contact);
		contacts.clear();
	}



private:
	std::vector<Contact> contacts;

	void detectCollision(Collider* colliderA , Collider* colliderB) {
	
	}

	void handleCollision(Contact& collision) {
	
	}


	void AABBxAABBCollision() {
	
	}

	void AABBxPlaneCollision() {
	
	}

	void AABBxSphereCollision() {

	}

	void SpherexSphereCollision() {
	
	}

	void SpherexPlaneCollision() {
	
	}

};