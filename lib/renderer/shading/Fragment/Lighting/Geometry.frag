#version 330 core

in FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in ;


struct Material {
bool useTextureAlbedo;
bool useTextureNormal;
bool useTextureHeight;
bool useTextureMetallicRoughness;
bool useTextureMetallic;
bool useTextureRoughness;
bool useTextureAmbientOcclusion;
bool useTextureEmissive;

vec3 albedo;
vec3 emissive;
float metallic;
float roughness;
float ambientOcclusion;
float heightScale;

sampler2D textureAlbedo;
sampler2D textureNormal;
sampler2D textureHeight;
sampler2D textureMetallicRoughness;
sampler2D textureMetallic;
sampler2D textureRoughness;
sampler2D textureAmbientOcclusion;
sampler2D textureEmissive;
};
uniform Material material;
uniform vec3 viewPos;


layout (location=0) out vec3 FragPosition;
layout (location=1) out vec3 FragNormal;
layout (location=2) out vec3 FragColor;
layout (location=3) out vec3 FragMaterial;       // roughness + metalic + ambient
layout (location=4) out vec3 FragEmissive;


float parallaxHeight = 0.0;
vec2 RaymarchedParallaxMapping(vec2 TexCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 64.0;
    
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    vec2 P = viewDir.xy * material.heightScale;
    vec2 deltaTexCoords = P / numLayers;
    
    vec2 currentTexCoords = TexCoords;
    float currentLayerDepth = 0.0;
    float depthMapValue = 1.0 - texture(material.textureHeight, currentTexCoords).r;
    
    while (currentLayerDepth < depthMapValue) {
        currentTexCoords -= deltaTexCoords;
        depthMapValue = 1.0 - texture(material.textureHeight, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    
    // Binary search refinement
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = depthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(material.textureHeight, prevTexCoords).r) - (currentLayerDepth - layerDepth);
    float weight = afterDepth / (afterDepth - beforeDepth);
    
    parallaxHeight = 1.0 - mix(
        texture(material.textureHeight, currentTexCoords).r,
        texture(material.textureHeight, prevTexCoords).r,
        weight
    );
    return mix(currentTexCoords, prevTexCoords, weight);
}



void main() {
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec2 TexCoords = fs_in.TexCoords;
    if (material.useTextureHeight) {
        TexCoords = RaymarchedParallaxMapping(fs_in.TexCoords,normalize(transpose(fs_in.TBN) * viewDir));
        //if(TexCoords.x > uvMultiplier.x || TexCoords.y > uvMultiplier.y || TexCoords.x < 0.0 || TexCoords.y < 0.0)
        //discard;
    }
    
    FragNormal = fs_in.TBN[2];
    if (material.useTextureNormal) {
        vec3 tangentNormal = texture(material.textureNormal, TexCoords).xyz * 2.0 - 1.0;
        FragNormal = normalize(fs_in.TBN * tangentNormal);
    }

    // Get the final height at the displaced texcoords
    FragPosition = fs_in.FragPos;
    if (material.useTextureHeight) {
        parallaxHeight *= material.heightScale;
        vec3 tangentDisplacement = vec3(0.0, 0.0, -parallaxHeight);  // Negative because heightmap is "1" at peaks
        vec3 worldDisplacement = fs_in.TBN * tangentDisplacement;
        FragPosition += worldDisplacement;
    }

    // albedo
    FragColor = material.albedo;
    if (material.useTextureAlbedo) {
        vec4 albedo = texture(material.textureAlbedo, TexCoords);
        if (albedo.w < 0.1) discard;
		FragColor = albedo.rgb;
	}

    // metallic/roughness
	float metallic = material.metallic;
	float roughness = material.roughness;
	if (material.useTextureMetallicRoughness) {
		vec3 metallicRoughness = texture(material.textureMetallicRoughness, TexCoords).rgb;
		metallic = metallicRoughness.b;
		roughness = metallicRoughness.g;
	}
    else {
        if (material.useTextureMetallic) {
            metallic = texture(material.textureMetallic, TexCoords).r;
        }
        if (material.useTextureRoughness) {
            roughness = texture(material.textureRoughness, TexCoords).r;
        }
    }

    // ambient occlusion
    float ao = material.ambientOcclusion;
	if (material.useTextureAmbientOcclusion) {
		ao = texture(material.textureAmbientOcclusion, TexCoords).r;
	}

    FragMaterial = vec3(roughness,metallic,ao);


    // emissive
	vec3 emissive = material.emissive;
	if (material.useTextureEmissive) {
		emissive = texture(material.textureEmissive, TexCoords).rgb;
	}
    FragEmissive = emissive;
}