#version 330 core

const float PI = 3.14159265359;

struct Light {
    mat4 viewProjectionMatrix;
    vec3 position;
    vec3 center;
    vec3 axes[3];
    float halfSizes[3];
    vec3 color;
    float intensity;
    bool unified;
};
uniform Light light;
uniform vec3 viewPos;
uniform vec2 pixelSize;
uniform float bias;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMaterial;
uniform sampler2D depthMap;

out vec4 FragColor;


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float shadowCalculation(vec3 fragPos,vec3 normal)
{
    vec4 fragmentPositionLightSpace = light.viewProjectionMatrix * vec4(fragPos,1.0);
    vec3 projCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Oversampling
    if (projCoords.z > 1.0) {return 0.0;}

    float currentDepth = projCoords.z;

    // Shadow Acne
    float bias = max(bias * (1.0 - dot(normal, light.axes[2])), bias/10);  

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
        for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }  
    shadow /= 9.0;

    return shadow;
}


void main() {
    // Reconstructing World Coordinantes
    vec2 TexCoords = vec2(( gl_FragCoord.x * pixelSize.x) ,(gl_FragCoord.y * pixelSize.y));
    vec3 pos = vec3(texture(gPosition , TexCoords));
    
    // Light Distance Check
    vec3 d = pos - light.center;
    for (int i = 0; i < 3; ++i) {
	float dist = dot(d, light.axes[i]);
	if (abs(dist) > light.halfSizes[i])
        discard;
    }
    
    // Compute Distance
    float distance = dot(light.axes[2], pos - light.center) + light.halfSizes[2];
    
    // Screen Space To World
    vec3 V = normalize(viewPos - pos);
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 material = texture(gMaterial, TexCoords).rgb;
    float roughness = material.r;
    float metallic  = material.g;
    float ao        = material.b;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = -normalize(light.axes[2]);
    vec3 H = normalize(V + L);
    float attenuation = light.unified ? 1.0 : (1.0 / (distance * distance));
    vec3 radiance = light.color * light.intensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    float illuminated = 1.0 - shadowCalculation(pos,N);

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ( ao == 0.0 ? 1.0 : ao );
    vec3 color = ambient + Lo * illuminated;
    
    FragColor = vec4(color,1.0);
    //FragColor = vec4(1.0);
}