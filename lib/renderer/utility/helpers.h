#pragma once
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>


class AssimpGLMHelpers
{
public:

	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};


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


void printMat4(const glm::mat4& matrix, int precision = 4, int colWidth = 10) {
    std::cout << endl;

    // Save original stream settings
    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);

    // Configure output
    std::cout << std::fixed << std::setprecision(precision);

    // Print each row
    for (int row = 0; row < 4; ++row) {
        std::cout << "| ";
        for (int col = 0; col < 4; ++col) {
            std::cout << std::setw(colWidth) << matrix[col][row] << " "; // Note: GLM is column-major!
        }
        std::cout << " |" << std::endl;
    }

    // Restore stream settings
    std::cout.copyfmt(oldState);

    std::cout << endl;
}