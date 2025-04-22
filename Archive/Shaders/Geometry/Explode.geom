#version 330 core

layout(triangles) in;
layout(triangle_strip , max_vertices = 3) out;

in GEO {
    vec3 normal;
    vec3 fragmentPosition;
    vec2 texCoord;
} geoIn[] ;
uniform float time;

out GEO {
    vec3 normal;
    vec3 fragmentPosition;
    vec2 texCoord;
} geoOut ;


vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

const float magnitude = 2.0;

void main () {
    vec3 normal = GetNormal();
    geoOut.normal = geoIn[0].normal;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    for (int i = 0 ; i<3 ; i++) {
        gl_Position = gl_in[i].gl_Position + vec4(direction,0.0);
        geoOut.fragmentPosition = geoIn[i].fragmentPosition;
        geoOut.texCoord = geoIn[i].texCoord;
        EmitVertex();
    }
    EndPrimitive();
}