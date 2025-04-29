#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../shading/Shader.h"
#include "Object3d.h"
#include "Geometry.h"
#include "Material.h"



class Mesh : public Object3D {
public:
    unsigned int VAO, VBO, EBO;
    Material material;
    Geometry geometry;


    Mesh(Geometry geometry, Material material) : geometry(geometry) , material(material) {
        // create our data structures
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // use this VAO for subsequent calls

        glBindBuffer(GL_ARRAY_BUFFER, VBO); // use this VBO for subsequent calls
        glBufferData(GL_ARRAY_BUFFER, geometry.vertices.size() * sizeof(Vertex), &geometry.vertices[0], GL_STATIC_DRAW); // copy over the vertex data

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // use this EBO for subsequent calls
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.indices.size() * sizeof(unsigned int), &geometry.indices[0], GL_STATIC_DRAW); // copy over the index data

        // setup the locations of vertex data
        // positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));

        // texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTextureCoordinates));

        // tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTangent));

        // bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mBitangent));

        glBindVertexArray(0);
    }



    virtual void render(Shader* shader, mat4 parentMatrix = mat4(1.0f), bool materialize = false, bool geometeryPass = false) {
        Object3D::render(shader, parentMatrix, materialize, geometeryPass);

        if (materialize)
            material.bind(shader);

        shader->setMat4("model", worldMatrix);
        shader->setMat3("normalMatrix", transpose(inverse(mat3(worldMatrix))));
        draw();
    }



    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

};