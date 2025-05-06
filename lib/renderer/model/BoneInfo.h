#pragma once
#include <glm/glm.hpp>


struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset = mat4(1);

	glm::mat4 finalTransformMatrix = mat4(1);
};