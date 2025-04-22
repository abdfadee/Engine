#version 330 core

uniform float znear = 0.1f , zfar = 100.0f; 
out vec4 FragColor;

vec4 getDepth () {
    return vec4(vec3(gl_FragCoord.z),1.0);
}

// Only when using perspective projection
vec4 getLinearizedDepth (float near , float far) {
    float ndc = gl_FragCoord.z * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - ndc * (far -near));
    return vec4(vec3(linearDepth / far),1.0);
}

void main() {
    FragColor = getDepth();
}