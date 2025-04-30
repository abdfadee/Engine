#version 330 core
in vec3 texDir;
out vec4 FragColor;

uniform samplerCube cube;

void main() { 
    FragColor = vec4(vec3(texture(cube,texDir)),1.0);
}