#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};

out FS_IN
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
} vsOut ;

void main() {
    vsOut.normal = normalize(vec3(transpose(inverse(model)) * vec4(Normal, 0.0)));
    vsOut.fragPos = vec3(model * vec4(position,1.0));
    vsOut.texCoord = TexCoord;
    gl_Position = projection * view * model * vec4(position,1.0);
}