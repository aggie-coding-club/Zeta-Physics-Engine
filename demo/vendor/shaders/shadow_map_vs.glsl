#version 300 es

in vec3 position;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main(void){
    vec4 world_position = transformation_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
}