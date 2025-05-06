#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../shading/Shader.h"
#include "../core/Mesh.h"


class Node {
public:
	glm::mat4 transformation;
    glm::mat4 globalTransformation;
	std::string name;
	std::vector<Node*> children;
    vector<Mesh*> meshes;

    void render(Shader* shader, glm::mat4 parentMatrix = glm::mat4(1.0f), bool materialize = false, bool geometeryPass = false , bool animate = false) {
        if (!animate)
            parentMatrix *= transformation;
        else
            shader->setMat4("fallbackMatrix", globalTransformation);

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i]->render(shader, parentMatrix, materialize, geometeryPass);

        for (unsigned int i = 0; i < children.size(); i++)
            children[i]->render(shader, parentMatrix, materialize, geometeryPass,animate);
    }
};