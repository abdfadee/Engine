#version 330 core

layout(triangles) in;
layout(line_strip , max_vertices = 6) out;

in GEO_IN {
    mat4 perspective;
    vec3 normal;
} geoIn[] ;

const float MAGNITUDE = 0.4;

void main () {
    for (int i = 0 ; i<3 ; i++) {
        gl_Position = geoIn[0].perspective * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = geoIn[0].perspective * (gl_in[i].gl_Position + vec4(geoIn[0].normal,0.0) * MAGNITUDE);
        EmitVertex();
        EndPrimitive();
    }
}