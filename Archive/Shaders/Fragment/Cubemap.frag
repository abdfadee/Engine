#version 330 core
in vec3 texDir;
out vec4 FragColor;

uniform samplerCube cube;

void main() { 
    //FragColor = textureLod(cube,texDir,1.2);
    FragColor = texture(cube,texDir);
}