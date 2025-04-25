#version 330 core
layout(location = 0) in vec3 aPosition;

layout(std140) uniform View {
    mat4 viewProjectionMatrix;
};
uniform mat4 model;

void main() {
    gl_Position = viewProjectionMatrix * model * vec4(aPosition,1.0);
}