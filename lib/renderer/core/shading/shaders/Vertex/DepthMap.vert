#version 330 core
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 offset;

uniform mat4 model;
uniform mat4 spaceMatrix = mat4(1);

void main() {
    gl_Position = spaceMatrix * model * vec4(position + offset,1.0);
}