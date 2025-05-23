#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../shading/Shader.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


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
    quat orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
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
        orientation = quat(eulerAngles);
        matrixNeedsUpdate = true;
    }

    void setRotation(quat q) {
        orientation = normalize(q);
        rotationVector = eulerAngles(orientation);
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
        orientation = quat(eulerAngles) * orientation;
        orientation = normalize(orientation);
        rotationVector = glm::eulerAngles(orientation);
        matrixNeedsUpdate = true;
    }

    void rotate(quat q) {
        orientation = normalize(q * orientation);
        rotationVector = glm::eulerAngles(orientation);
        matrixNeedsUpdate = true;
    }

    void updateLocalMatrixIfNeeded() {
        if (!matrixNeedsUpdate) return;
        localMatrix = mat4(1.0f);
        localMatrix = glm::translate(localMatrix, positionVector);
        localMatrix *= mat4_cast(orientation);
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
        mat4 worldMatrix = getWorldMatrix();
        return vec3(worldMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    quat  getWorldOrientation() {
        if (parent) {
            return parent->getWorldOrientation() * orientation;
        }
        return orientation;
    }

    virtual void render(Shader* shader, mat4 parentMatrix = mat4(1.0f),bool materialize = false , bool geometeryPass = false) {
        worldMatrix = parentMatrix * getLocalMatrix();

        for (auto* child : children)
            child->render(shader,worldMatrix,materialize,geometeryPass);
    }

};