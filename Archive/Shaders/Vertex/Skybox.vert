#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 perspect;
};

out vec3 texDir;

void main() {
    texDir = position;
    mat4 mview = mat4(mat3(view));
    gl_Position = perspect * mview * model * vec4(position,1.0);
    gl_Position = gl_Position.xyww;
}