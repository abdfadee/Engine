#version 330 core
in vec2 TexCoords;
uniform vec3 color;
uniform sampler2D tex;
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0);
    //FragColor = (color == vec3(0.0)) ? texture(tex,TexCoords) : vec4(color,1.0) ;
    //if (FragColor.w < 0.1)
        //discard;
}