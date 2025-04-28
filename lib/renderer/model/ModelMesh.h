#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../shading/Shader.h"





/**
 * A mesh is a collection of geometry paired with a material.
 */
class ModelMesh {
public:
    


    struct Texture {
        unsigned int mId;
        std::string mPath; // used to de-dupe textures loaded
    };


    


    ModelMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) : mVertices(vertices), mIndices(indices), mMaterial(material) {
        init();
    }

    void Draw(Shader* shader) {

        

        // draw mesh
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

public: 
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	Material mMaterial;

private:
    void init() {
        // create our data structures
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO); // use this VAO for subsequent calls

        glBindBuffer(GL_ARRAY_BUFFER, mVBO); // use this VBO for subsequent calls
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // copy over the vertex data

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO); // use this EBO for subsequent calls
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW); // copy over the index data

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

private:
    // OpenGL data structures
    unsigned int mVAO, mVBO, mEBO;
};