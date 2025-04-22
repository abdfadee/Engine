#version 330 core

in vec3 normal;
in vec3 fragmentPosition;
in vec4 fragmentPositionLightSpace;
in vec2 texCoord;

uniform vec3 lightColor = vec3(1);
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec4 color;
uniform float far_plane;

uniform sampler2D tex;
uniform sampler2D depthMap;
uniform samplerCube cubeDepthMap;

out vec4 FragColor;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);  


float OmnidirectionalShadowCalculation(vec3 fragPos,vec3 viewPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 200.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(cubeDepthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

float ShadowCalculation(vec4 fragmentPositionLightSpace,vec3 normal,vec3 lightDirection)
{
    vec3 projCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Oversampling
    if (projCoords.z > 1.0) {return 0.0;}

    //float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Shadow Acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);  


    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;

    //return currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
}  



void main() {
    vec3 cameraDirection = normalize(cameraPos-fragmentPosition);
    vec3 lightDirection = normalize(lightPos - fragmentPosition);
    vec3 halfwayDir = normalize(lightDirection + cameraDirection);

    //float light = 1.0 - ShadowCalculation(fragmentPositionLightSpace,normal,lightDirection);
    float light = 1.0 - OmnidirectionalShadowCalculation(fragmentPosition,cameraPos);

/*
    vec3 fragToLight = fragmentPosition - lightPos; 
    float closestDepth = texture(cubeDepthMap, fragToLight).r;
    closestDepth *= far_plane;
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);  
    return;
    */

    
    // Ambient
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = lightColor * diff * light;

    // Speculer
    float specularStrength = 2;
    float spec = pow(max(dot(normal,halfwayDir),0.0),128.0f);
    vec3 speculer = specularStrength * spec * lightColor * light;

    // Result
    vec4 tex = texture(tex,texCoord);
    FragColor = vec4(ambient + diffuse + speculer,1) * (tex + color);

    if (FragColor.w < 0.1)
        discard;
}