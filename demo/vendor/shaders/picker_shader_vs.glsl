#version 330 core

in vec3 position;
in vec3 tex_coords;
in vec3 normal;
in vec3 color;

out vec4 f_color;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform float identifier;

void main(void){
    vec4 world_position = transformation_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
    
    f_color = vec4(identifier, identifier, identifier, 1.0);
}  