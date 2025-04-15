#version 430 core

layout(triangles) in;
layout(line_strip , max_vertices = 6) out;

in GEO_IN {
    mat4 projection;
    vec3 normal;
} geoIn[] ;

const float MAGNITUDE = 0.4;

void main () {
    for (int i = 0 ; i<3 ; i++) {
        gl_Position = geoIn[0].projection * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = geoIn[0].projection * (gl_in[i].gl_Position + vec4(geoIn[i].normal,0.0) * MAGNITUDE);
        EmitVertex();
        EndPrimitive();
    }
}