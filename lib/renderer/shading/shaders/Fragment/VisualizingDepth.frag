#version 330 core

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float znear = 0.1f , zfar = 100.0f; 
uniform bool perspective;


vec4 getDepth () {
    return vec4(vec3(texture(depthMap,TexCoords).r),1.0);
}

// Only when using perspective projection
vec4 getLinearizedDepth () {
    float ndc = texture(depthMap,TexCoords).r * 2.0 - 1.0;
    float linearDepth = (2.0 * znear * zfar) / (zfar + znear - ndc * (zfar -znear));
    return vec4(vec3(linearDepth / zfar),1.0);
}

out vec4 FragColor;

void main() {
    FragColor = perspective ? getLinearizedDepth() : getDepth();
}