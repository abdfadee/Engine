#version 330 core

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform vec3 lightPos = vec3(1.5,0,0);
uniform vec3 lightColor = vec3(3);
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    vec3 fragPos = vec3(texture(gPosition,TexCoords));
    vec3 normal = vec3(texture(gNormal,TexCoords));
    vec4 color = texture(gAlbedo,TexCoords);

    vec3 viewDirection = normalize(viewPos-fragPos);
    vec3 lightDirection = normalize(lightPos - fragPos);
    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    // Ambient
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = diff * lightColor;

    // Speculer
    float specularStrength = 2;
    float spec = pow(max(dot(normal,halfwayDir),0.0),128.0f);
    vec3 speculer = specularStrength * spec * lightColor;

    // Result
    FragColor = vec4(ambient + diffuse + speculer,1) * color;

    if (FragColor.w < 0.1)
        discard;
}