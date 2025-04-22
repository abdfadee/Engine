#version 330 core
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec3 offset;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 perspect;
};

out vec2 TexCoords;

void main() {
    gl_Position = perspect * view * model * vec4(position + offset,1.0);
    TexCoords = TexCoord;
}