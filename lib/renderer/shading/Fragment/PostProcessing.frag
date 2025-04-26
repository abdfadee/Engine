#version 330 core

in vec2 TexCoords;

uniform sampler2D buffer;
uniform float exposure = 1.0;

out vec4 FragColor;


const float gamma = 2.2;
vec3 toneMapping () {
    vec3 hdrColor = texture(buffer,TexCoords).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    return pow(mapped,vec3(1.0/gamma));
}


void main() {
    FragColor = vec4(toneMapping(),1.0);
}