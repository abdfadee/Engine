#version 330 core

in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoord;

uniform vec3 lightColor = vec3(1);
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec4 color;
uniform sampler2D tex;

out vec4 FragColor;


void main() {
    vec3 cameraDirection = normalize(cameraPos-fragmentPosition);
    vec3 lightDirection = normalize(lightPos - fragmentPosition);
    vec3 halfwayDir = normalize(lightDirection + cameraDirection);

    // Ambient
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = lightColor * diff;

    // Speculer
    float specularStrength = 2;
    float spec = pow(max(dot(normal,halfwayDir),0.0),128.0f);
    vec3 speculer = specularStrength * spec * lightColor;

    // Result
    vec4 tex = texture(tex,texCoord);
    FragColor = vec4(ambient + diffuse + speculer,1) * (tex + color);

    if (FragColor.w < 0.1)
        discard;
}