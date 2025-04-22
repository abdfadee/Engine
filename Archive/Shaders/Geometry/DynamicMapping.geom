#version 330 core

layout(triangles) in;
layout(triangle_strip , max_vertices = 18) out;

in GEO_IN {
    vec3 normal;
    vec2 TexCoords;
} geo_in[];

uniform mat4 spaceMatrices[6];

out vec4 fragPos;
out vec3 normal;
out vec2 TexCoords;

void main () {
    for (int i = 0 ; i<6 ; i++) {
        gl_Layer = i; // built-in variable that specifies to which face we render.
        for (int j = 0 ; j<3 ; j++) {
            fragPos = gl_in[j].gl_Position;
            normal = geo_in[j].normal;
            TexCoords = geo_in[j].TexCoords;
            gl_Position = spaceMatrices[i] * fragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}