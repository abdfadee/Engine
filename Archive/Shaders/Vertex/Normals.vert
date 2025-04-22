#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 Normal;

uniform mat4 model;
layout(std140) uniform CameraBuffer {
    mat4 view;
    mat4 perspect;
};

out GEO_IN {
    mat4 perspective;
    vec3 normal;
} geoIn ;


void main() {
    gl_Position = view * model * vec4(position,1.0);
    geoIn.normal = vec3(transpose(inverse(view * model)) * vec4(Normal, 0.0));
    geoIn.perspective = perspect;
}