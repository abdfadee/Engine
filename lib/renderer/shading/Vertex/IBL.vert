#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 viewProjectionMatrix;

void main()
{
    WorldPos = aPos;  
    gl_Position =  viewProjectionMatrix * vec4(WorldPos, 1.0);
}