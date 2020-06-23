#version 130

attribute vec3 vertex;
attribute vec3 normal;

uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;

//flat out vec3 frag_normal;
varying vec3 frag_normal;
varying mat4 mvp_matrix;

void main() {
    mvp_matrix = projection_matrix * modelview_matrix;
    gl_Position = mvp_matrix * vec4(vertex, 1.0);
    frag_normal = normal;
}
