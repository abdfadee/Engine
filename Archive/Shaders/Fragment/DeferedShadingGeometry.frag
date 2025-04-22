#version 330 core

in FS_IN {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in ;

uniform sampler2D diffuseMap;

layout (location=0) out vec4 FragPos;
layout (location=1) out vec4 FragNormal;
layout (location=2) out vec4 FragColor;

void main() {
    FragPos = vec4(fs_in.FragPos,1.0);
    FragNormal = vec4(fs_in.Normal,1.0);
    FragColor = texture(diffuseMap,fs_in.TexCoords);
    if (FragColor.w < 0.1)
        discard;
}