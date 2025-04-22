#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 perspect;
};

uniform vec3 lightPos;
uniform vec3 viewPos;


out FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out ;


void main() {
    mat4 normalMatrix = transpose(inverse(model));
    vec3 T = normalize(vec3(normalMatrix * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(normalMatrix * vec4(aNormal,    0.0)));
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    TBN = transpose(TBN);

    vs_out.FragPos = vec3(model * vec4(aPosition,1.0));
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TexCoords = aTexCoords;
    
    gl_Position = perspect * view * model * vec4(aPosition,1.0);
}