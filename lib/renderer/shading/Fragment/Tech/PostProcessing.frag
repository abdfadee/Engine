#version 330 core

in vec2 TexCoords;

uniform float exposure = 1.0;
uniform sampler2D scene;
uniform sampler2D blur;

out vec4 FragColor;


const float gamma = 2.2;
vec3 toneMapping (vec3 color) {
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    return pow(mapped,vec3(1.0/gamma));
}


void main() {
    vec3 color = texture(scene,TexCoords).rgb;
    vec3 blur = texture(blur,TexCoords).rgb;
    color += blur;
    FragColor = vec4(toneMapping(color),1.0);
}