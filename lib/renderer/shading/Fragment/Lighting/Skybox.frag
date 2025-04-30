#version 330 core
in vec3 texDir;

uniform samplerCube cube;

layout (location=2) out vec3 FragColor;

void main() { 
    FragColor = vec3(texture(cube,texDir));
}