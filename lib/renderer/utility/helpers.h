#pragma once
#include <iostream>
#include <glm/glm.hpp>


std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


bool parallelismCheck(glm::vec3 a , glm::vec3 b) {
	glm::vec3 na = glm::normalize(a);
	glm::vec3 nb = glm::normalize(b);
	bool areParallel = (length(na - nb) < 0.0001) || (length(na + nb) < 0.0001);
	areParallel = areParallel && length(cross(a, b)) < 0.0001;
	return areParallel;
}