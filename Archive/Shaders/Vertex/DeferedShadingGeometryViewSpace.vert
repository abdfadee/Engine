#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};

out FS_IN {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out ;

uniform bool invertedNormals = false;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    vs_out.FragPos = viewPos.xyz; 
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vs_out.Normal = normalize(normalMatrix * (invertedNormals ? -aNormal : aNormal));
    
    gl_Position = projection * viewPos;
}