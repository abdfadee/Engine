#pragma once

#include <glad/glad.h>
#include "Object3d.h"
#include "../Material.h"
#include "../Geometry.h"


class Mesh : public Object3D {
public:
    GLuint VAO, VBO, EBO;
    Geometry* geometry;
    Material* material;


    Mesh(Geometry* geometry, Material* material = nullptr) : geometry(geometry), material(material) {
        // Generate and bind the VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind the VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, geometry->buffer.size() * sizeof(GLfloat), geometry->buffer.data(), GL_STATIC_DRAW);

        // Set up vertex attribute pointers
        // Position (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinates (location = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Tangent (location = 3)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);

        // Bitangent (location = 4)
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
        glEnableVertexAttribArray(4);

        // Generate and bind the EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry->indices.size() * sizeof(unsigned int), geometry->indices.data(), GL_STATIC_DRAW);

        // Unbind the VAO
        glBindVertexArray(0);
    }


    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, geometry->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    virtual void render(Shader* shader , mat4 parentMatrix = mat4(1.0f), bool materialize = true) {
        Object3D::render(shader,parentMatrix);

        if (materialize)
            material->bind(shader);
        
        shader->setMat4("model", worldMatrix);
        shader->setMat3("normalMatrix", transpose(inverse(mat3(worldMatrix))));
        draw();
    }

};