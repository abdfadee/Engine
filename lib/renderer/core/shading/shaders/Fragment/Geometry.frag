#version 330 core

in FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in ;

layout(std140) uniform View {
    mat4 viewProjectionMatrix;
};

uniform vec3 viewPos;
uniform vec3 color = vec3(0.0);
uniform float metallic;
uniform float roughness;
uniform float ambientOcclusion;
uniform float heightScale;
uniform vec2 uvMultiplier;

uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;
uniform sampler2D displacmentMap;
uniform sampler2D aoMap;

layout (location=0) out vec3 FragPosition;
layout (location=1) out vec3 FragNormal;
layout (location=2) out vec3 FragColor;
layout (location=3) out vec3 FragMaterial;       // roughness + metalic + ambient


float parallaxHeight = 0.0;

vec2 RaymarchedParallaxMapping(vec2 TexCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 64.0;
    
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numLayers;
    
    vec2 currentTexCoords = TexCoords;
    float currentLayerDepth = 0.0;
    float depthMapValue = 1.0 - texture(displacmentMap, currentTexCoords).r;
    
    while (currentLayerDepth < depthMapValue) {
        currentTexCoords -= deltaTexCoords;
        depthMapValue = 1.0 - texture(displacmentMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    
    // Binary search refinement
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = depthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(displacmentMap, prevTexCoords).r) - (currentLayerDepth - layerDepth);
    float weight = afterDepth / (afterDepth - beforeDepth);
    
    parallaxHeight = 1.0 - mix(
        texture(displacmentMap, currentTexCoords).r,
        texture(displacmentMap, prevTexCoords).r,
        weight
    );
    return mix(currentTexCoords, prevTexCoords, weight);
}



void main() {
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec2 TexCoords = fs_in.TexCoords;
    if (heightScale > 0.0) {
        TexCoords = RaymarchedParallaxMapping(fs_in.TexCoords,normalize(transpose(fs_in.TBN) * viewDir));
        if(TexCoords.x > uvMultiplier.x || TexCoords.y > uvMultiplier.y || TexCoords.x < 0.0 || TexCoords.y < 0.0)
        discard;

        vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
        FragNormal = normalize(fs_in.TBN * tangentNormal);
    }
    else
        FragNormal = fs_in.TBN[2];

    // Get the final height at the displaced texcoords
    parallaxHeight *= heightScale;
    vec3 tangentDisplacement = vec3(0.0, 0.0, -parallaxHeight);  // Negative because heightmap is "1" at peaks
    vec3 worldDisplacement = fs_in.TBN * tangentDisplacement;
    FragPosition = fs_in.FragPos + worldDisplacement;
    
    //vec4 clip = viewProjectionMatrix * vec4(FragPosition,1.0);
    //gl_FragDepth = (clip.z / clip.w);

    FragColor = color + texture(albedoMap, TexCoords).xyz;

    float r = roughness + texture(roughnessMap, TexCoords).r;
    float m = metallic + texture(metallicMap, TexCoords).r;
    float ao = ambientOcclusion + texture(aoMap, TexCoords).r;
    FragMaterial = vec3(r,m,ao);
}