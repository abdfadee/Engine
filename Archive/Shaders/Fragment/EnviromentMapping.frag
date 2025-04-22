#version 330 core

in vec3 normal;
in vec3 fragmentPosition;

uniform bool ReflectOrRefract = true;
uniform vec3 viewPos;
uniform samplerCube cube;

out vec4 FragColor;

void main () {
    vec3 cameraDirection = normalize(fragmentPosition - viewPos);
    vec3 r;

    if (ReflectOrRefract) {
        /* Reflection */
        r = reflect(cameraDirection,normalize(normal));
    }
    else {
        /* Refraction */
        float ratio = 1.00 / 1.52;
        r = refract(cameraDirection,normalize(normal),ratio);
    }

    FragColor = texture(cube,r);
}