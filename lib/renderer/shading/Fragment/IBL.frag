#version 330 core

in vec2 TexCoords;

uniform vec3 viewPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMaterial;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

out vec4 FragColor;


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  


void main() {
    // Reconstructing World Coordinantes
    vec3 pos = vec3(texture(gPosition , TexCoords));
    
    if (pos == vec3(0)) {
        FragColor = vec4(vec3(texture(gAlbedo,TexCoords)),1.0);
        return;
    }
    
    
    // Screen Space To World
    vec3 V = normalize(viewPos - pos);
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 R = reflect(-V, N); 
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 material = texture(gMaterial, TexCoords).rgb;
    float roughness = material.r;
    float metallic  = material.g;
    float ao        = material.b;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular);
    
    FragColor = vec4(ambient,1.0); 
    //FragColor = vec4(diffuse,1.0); 
    //FragColor = vec4(texture(prefilterMap, N).rgb,1.0); 
    //FragColor = vec4(texture(brdfLUT, TexCoords).rgb,1.0); 
}