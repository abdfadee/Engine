#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;


void gamaCorrect ();
void hdrToneMapping();



void main() { 
    vec4 color = texture(scene,TexCoords);
    vec3 blur = texture(bloomBlur,TexCoords).rgb;
    FragColor = vec4(color.rgb + blur,color.a);
    hdrToneMapping();
    gamaCorrect();
}


void gamaCorrect () {
    FragColor = vec4(pow(FragColor.rgb,vec3(1/2.2)),FragColor.a);
}


void hdrToneMapping () {
    // reinhard tone mapping
    FragColor = vec4(vec3(1.0) - exp(-FragColor.rgb * exposure),FragColor.a);
}