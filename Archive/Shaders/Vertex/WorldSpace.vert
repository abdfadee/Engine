#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 model;

out GEO_IN {
    vec3 normal;
    vec2 TexCoords;
} vs_out;

void main() {
    vs_out.normal = normalize(vec3(transpose(inverse(model)) * vec4(Normal, 0.0)));
    vs_out.TexCoords = TexCoord;
    gl_Position = model * vec4(position,1.0);
}