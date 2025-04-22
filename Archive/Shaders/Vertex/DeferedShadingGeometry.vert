#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

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

void main() {
    vs_out.FragPos = vec3(model * vec4(Position,1.0));
    vs_out.Normal = normalize(vec3(transpose(inverse(model)) * vec4(Normal, 0.0)));
    vs_out.TexCoords = TexCoords;
    gl_Position = projection * view * model * vec4(Position,1.0);
}