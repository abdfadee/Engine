#version 330 core

const float PI = 3.14159265359;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};
uniform Light light;
uniform vec2 pixelSize;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gDepth;

out vec4 FragColor;



void main() {
    // Reconstructing World Coordinantes
    vec2 TexCoords = vec2(( gl_FragCoord.x * pixelSize.x) ,(gl_FragCoord.y * pixelSize.y));
    vec3 pos = vec3(texture(gPosition , TexCoords));
    
    // Light Distance Check
    float distance = length(light.position-pos);
    float atten = 1.0 - clamp (distance / light.radius , 0.0 , 1.0);
    if (atten == 0.0)
        discard;
    
    float attenuation = 1.0 / (distance * distance);
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 ambient = vec3(0.35) * albedo * light.color * light.intensity * attenuation;
    
    FragColor = vec4(ambient,1.0);
    //FragColor = vec4(1.0);
}