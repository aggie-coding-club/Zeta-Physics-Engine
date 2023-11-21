#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 tex_coords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;

out vec3 f_color;
out vec3 f_tex_coords;
out vec3 f_surface_normal;

out vec3 f_current_position;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform vec4 u_color;

void main(void){
    vec4 world_position = transformation_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
    
    f_tex_coords = vec3(tex_coords.x, tex_coords.y, tex_coords.z);

    f_surface_normal = mat3(transpose(inverse(transformation_matrix))) * normal;
    
    f_color = color * u_color.xyz;
    f_current_position = (world_position).xyz;
}