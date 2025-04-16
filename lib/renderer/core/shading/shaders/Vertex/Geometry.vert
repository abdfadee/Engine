#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform mat3 normalMatrix;
uniform float uvMultiplier = 3.0;


out FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out ;


void main() {
    vs_out.FragPos = vec3(model * vec4(aPosition,1.0));
    vs_out.TexCoords = uvMultiplier * aTexCoords;
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vs_out.TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(vs_out.FragPos,1.0);
}