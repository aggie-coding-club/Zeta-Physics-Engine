#version 300 es

in vec3 position;
in vec2 tex_coords;
in vec3 normal;
in vec3 color;

out vec3 f_color;
out vec2 f_tex_coords;
out vec3 f_surface_normal;
out vec3 f_to_light_vec;
out vec3 f_to_camera_vec;
out vec4 f_light_color;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform vec3 light_position;
uniform vec4 u_color;

void main(void){
    vec4 world_position = transformation_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
    
    f_tex_coords = tex_coords;

    f_surface_normal = (transformation_matrix * vec4(normal, 0.0)).xyz;
    f_to_light_vec = light_position - world_position.xyz;
    f_to_camera_vec = (inverse(view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - world_position.xyz;

    f_color = u_color.xyz;
}