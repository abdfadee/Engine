#version 330 core

in FS_IN
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
} fsIn;


struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
    mat4 spaceMatrix;
    sampler2D depthMap;
};

struct PointLight {
    float far_plane;
    float constant;
    float linear;
    float quadratic;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
    samplerCube depthCubeMap;
};

struct SpotLight {
    float innerCutOff;
    float outerCutOff;
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
    mat4 spaceMatrix;
    sampler2D depthMap;
};

vec3 processLight (DirectionalLight light, vec3 viewDirection);
vec3 processLight (PointLight light, vec3 viewDirection);
vec3 processLight (SpotLight light, vec3 viewDirection);


uniform vec3 color;
uniform vec3 viewPos;
uniform DirectionalLight directional_lights[2];
uniform PointLight point_lights[3]; 
uniform SpotLight spot_lights[1];
uniform int directional_lights_num = 0 , point_lights_num = 0 , spot_lights_num = 0;
uniform sampler2D tex;


out layout (location = 0) vec4 FragColor;
out layout (location = 1) vec4 BrightColor;



void main() {
    vec3 viewDirection = normalize(viewPos-fsIn.fragPos);

    vec3 result = vec3(0.0);

    
    for (int i = 0 ; i<directional_lights.length() ; ++i)
        if (directional_lights[i].direction != vec3(0))
            result += processLight(directional_lights[i],viewDirection);
    
    for (int i = 0 ; i<point_lights.length() ; ++i)
        if (point_lights[i].constant != 0)
            result += processLight(point_lights[i],viewDirection);
    
    for (int i = 0 ; i<spot_lights.length() ; ++i)
        if (spot_lights[i].innerCutOff != 0.0)
        result += processLight(spot_lights[i],viewDirection);
        
    
    // Result
    vec4 tex = texture(tex,fsIn.texCoord);
    FragColor = (color == vec3(0.0)) ? vec4(result,1) * tex : vec4(color,1.0);

    if (FragColor.w < 0.1)
        discard;

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}




// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);  



float omnidirectionalShadowCalculation(vec3 lightPos,float far_plane,samplerCube depthCubeMap)
{
    vec3 fragToLight = fsIn.fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.05;
    float viewDistance = length(viewPos - fsIn.fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 200.0;
    for(int i = 0; i < gridSamplingDisk.length(); ++i)
    {
        float closestDepth = texture(depthCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(gridSamplingDisk.length()); 
    
    return shadow;
}

float directionalShadowCalculation(vec3 lightDirection,mat4 spaceMatrix,sampler2D depthMap)
{
    vec4 fragmentPositionLightSpace = spaceMatrix * vec4(fsIn.fragPos,1.0);
    vec3 projCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Oversampling
    if (projCoords.z > 1.0) {return 0.0;}

    float currentDepth = projCoords.z;

    // Shadow Acne
    float bias = max(0.05 * (1.0 - dot(fsIn.normal, lightDirection)), 0.005);  

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



vec3 processLight (DirectionalLight light, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    float illuminated = 1.0 - directionalShadowCalculation(lightDirection,light.spaceMatrix,light.depthMap);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(fsIn.normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff * illuminated;

    // Speculer
    float spec = pow(max(dot(fsIn.normal,halfwayDir),0.0),256.0f);
    vec3 speculer = light.speculer * spec * illuminated;

    // Result
    return ambient + diffuse + speculer ;
}


vec3 processLight (PointLight light, vec3 viewDirection) {
    vec3 lightDirection = light.position - fsIn.fragPos;
    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    float illuminated = 1.0 - omnidirectionalShadowCalculation(light.position,light.far_plane,light.depthCubeMap);

    float d = length(lightDirection);
    //float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * pow(d,2));
    float attenuation = 1.0 / (d * d);
    lightDirection = normalize(lightDirection);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(fsIn.normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff * illuminated;

    // Speculer
    float spec = pow(max(dot(fsIn.normal,halfwayDir),0.0),128.0f);
    vec3 speculer = light.speculer * spec * illuminated;

    // Result
    return attenuation * (ambient + diffuse + speculer);
}


vec3 processLight (SpotLight light, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fsIn.fragPos);
    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    float illuminated = 1.0 - directionalShadowCalculation(lightDirection,light.spaceMatrix,light.depthMap);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(fsIn.normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff * illuminated;

    // Speculer
    float spec = pow(max(dot(fsIn.normal,halfwayDir),0.0),128.0f);
    vec3 speculer = light.speculer * spec * illuminated;

    // spotlight (soft edges)
    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    speculer *= intensity;


    /*
    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;  
    */


    // Result
    return (ambient + diffuse + speculer);
}