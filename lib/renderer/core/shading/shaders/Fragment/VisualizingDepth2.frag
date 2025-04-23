#version 330 core

uniform float znear = 0.1f , zfar = 100.0f; 
uniform bool perspective;
out vec4 FragColor;

vec4 getDepth () {
    return vec4(vec3(gl_FragCoord.z),1.0);
}

// Only when using perspective projection
vec4 getLinearizedDepth () {
    float ndc = gl_FragCoord.z * 2.0 - 1.0;
    float linearDepth = (2.0 * znear * zfar) / (zfar + znear - ndc * (zfar -znear));
    return vec4(vec3(linearDepth / zfar),1.0);
}

void main() {
    FragColor = perspective ? getLinearizedDepth() : getDepth();
}