#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
};


struct PointLight {
    float constant;
    float linear;
    float quadratic;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
};


struct SpotLight {
    float innerCutOff;
    float outerCutOff;
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 speculer;
};


vec3 processLight (DirectionalLight light, vec3 cameraDirection);
vec3 processLight (PointLight light, vec3 cameraDirection);
vec3 processLight (SpotLight light, vec3 cameraDirection);




in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoord;

uniform vec3 cameraPos;
uniform vec4 color;
uniform sampler2D tex;

out vec4 FragColor;

uniform DirectionalLight directional_lights[2];
uniform PointLight point_lights[2]; 
uniform SpotLight spot_lights[2];


void main() {
    vec3 cameraDirection = normalize(cameraPos-fragmentPosition);

    vec3 result = vec3(0.0);

    for (int i = 0 ; i<directional_lights.length() ; ++i) {
        if (directional_lights[i].direction != vec3(0)) {
        result += processLight(directional_lights[i],cameraDirection);
        }
    }

    for (int i = 0 ; i<point_lights.length() ; ++i) {
        if (point_lights[i].constant != 0) {
        result += processLight(point_lights[i],cameraDirection);
        }
    }

    for (int i = 0 ; i<spot_lights.length() ; ++i) {
        if (spot_lights[i].innerCutOff != 0) {
        result += processLight(spot_lights[i],cameraDirection);
        }
    }
    
    // Result
    vec4 tex = texture(tex,texCoord);
    FragColor = vec4(result,1) * (tex + color);

    if (FragColor.w < 0.1)
        discard;
}




vec3 processLight (DirectionalLight light, vec3 cameraDirection) {
    vec3 lightDirection = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff;

    // Speculer
    vec3 lightReflection = reflect(-lightDirection,normal);
    float spec = pow(max(dot(cameraDirection,lightReflection),0.0),32.0f);
    vec3 speculer = light.speculer * spec;

    // Result
    return ambient + diffuse + speculer ;
}

vec3 processLight (PointLight light, vec3 cameraDirection) {
    vec3 lightDirection = light.position - fragmentPosition;
    float d = length(lightDirection);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * pow(d,2));
    lightDirection = normalize(lightDirection);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff;

    // Speculer
    vec3 lightReflection = reflect(-lightDirection,normal);
    float spec = pow(max(dot(cameraDirection,lightReflection),0.0),32.0f);
    vec3 speculer = light.speculer * spec;

    // Result
    return attenuation * (ambient + diffuse + speculer);
}

vec3 processLight (SpotLight light, vec3 cameraDirection) {
    vec3 lightDirection = normalize(light.position - fragmentPosition);

    // Ambient
    vec3 ambient = light.ambient;

    // Difuse
    float diff = max(dot(normal,lightDirection),0.0);
    vec3 diffuse = light.diffuse * diff;

    // Speculer
    vec3 lightReflection = reflect(-lightDirection,normal);
    float spec = pow(max(dot(cameraDirection,lightReflection),0.0),32.0f);
    vec3 speculer = light.speculer * spec;

    // spotlight (soft edges)
    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    speculer *= intensity;

    // Result
    return (ambient + diffuse + speculer);
}