#version 330 core

layout(triangles) in;
layout(triangle_strip , max_vertices = 18) out;

uniform mat4 spaceMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main () {
    for (int i = 0 ; i<6 ; i++) {
        gl_Layer = i; // built-in variable that specifies to which face we render.
        for (int j = 0 ; j<3 ; j++) {
            FragPos = gl_in[j].gl_Position;
            gl_Position = spaceMatrices[i] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}