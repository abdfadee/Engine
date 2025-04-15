#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../shading/Shader.h"

using namespace std;
using namespace glm;


class Object3D {
public:
    Object3D* parent = nullptr;
    vector<Object3D*> children;
    mat4 localMatrix = mat4(1.0f);
    mat4 worldMatrix = mat4(1.0f);
    vec3 scaleVector = vec3(1.0f);
    vec3 positionVector = vec3(0.0f);
    vec3 rotationVector = vec3(0.0f);
    bool matrixNeedsUpdate = true;


    void add(Object3D* object) {
        if (!object) {
            cerr << "Cannot add null object as child." << endl;
            return;
        }
        for (auto* child : children) {
            if (child == object) {
                cerr << "Object already exists in children." << endl;
                return;
            }
        }
        object->parent = this;
        children.push_back(object);
    }

    void remove(Object3D* object) {
        for (auto it = children.begin(); it != children.end(); ++it) {
            if (*it == object) {
                (*it)->parent = nullptr;
                children.erase(it);
                return;
            }
        }
        cerr << "Object not found in children." << endl;
    }

    void setPosition(vec3 position) {
        positionVector = position;
        matrixNeedsUpdate = true;
    }

    void setRotation(vec3 eulerAngles) {
        rotationVector = eulerAngles;
        matrixNeedsUpdate = true;
    }

    void scale(vec3 s) {
        scaleVector *= s;
        matrixNeedsUpdate = true;
    }

    void translate(vec3 t) {
        positionVector += t;
        matrixNeedsUpdate = true;
    }

    void rotate(vec3 eulerAngles) {
        rotationVector += eulerAngles;
        matrixNeedsUpdate = true;
    }

    void updateLocalMatrixIfNeeded() {
        if (!matrixNeedsUpdate) return;
        localMatrix = mat4(1.0f);
        localMatrix = glm::translate(localMatrix, positionVector);
        localMatrix = glm::rotate(localMatrix, rotationVector.z, vec3(0, 0, 1));
        localMatrix = glm::rotate(localMatrix, rotationVector.y, vec3(0, 1, 0));
        localMatrix = glm::rotate(localMatrix, rotationVector.x, vec3(1, 0, 0));
        localMatrix = glm::scale(localMatrix, scaleVector);
        matrixNeedsUpdate = false;
    }

    mat4 getLocalMatrix() {
        updateLocalMatrixIfNeeded();
        return localMatrix;
    }

    mat4 getWorldMatrix() {
        updateLocalMatrixIfNeeded();
        if (parent) {
            return parent->getWorldMatrix() * localMatrix;
        }
        return localMatrix;
    }

    vec3 getWorldPosition() {
        return vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    virtual void render(Shader* shader, mat4 parentMatrix = mat4(1.0f),bool materialize = true) {
        worldMatrix = parentMatrix * getLocalMatrix();

        for (auto* child : children)
            child->render(shader,worldMatrix);
    }

};