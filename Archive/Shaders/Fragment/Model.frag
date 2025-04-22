#version 330 core

in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoord;

#define textureLimit 4

struct Material {
    vec3 base;
    vec3 diffuse;
};
uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_speculer1;
uniform sampler2D texture_speculer2;
uniform sampler2D texture_speculer3;
uniform sampler2D texture_speculer4;

out vec4 FragColor;

void main() { 
    vec4 diffuse = texture(texture_diffuse1,texCoord);
    //vec4 speculer = texture(material.texture_speculer0,texCoord);
    FragColor = diffuse;
}