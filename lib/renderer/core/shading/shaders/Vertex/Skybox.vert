#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 mviewProjectionMatrix;

out vec3 texDir;

void main() {
    texDir = position;
    gl_Position = mviewProjectionMatrix * vec4(position,1.0);
    gl_Position = gl_Position.xyww;
}