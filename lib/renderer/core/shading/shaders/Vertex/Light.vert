#version 330 core
layout(location = 0) in vec3 aPosition;

layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

out mat4 inverseProjView;

void main() {
    inverseProjView = inverse(projection * view);
    gl_Position = projection * view * model * vec4(aPosition,1.0);
}