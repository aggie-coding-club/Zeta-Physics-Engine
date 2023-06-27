#version 300 es

// add support for texturing later
in vec2 position;
in vec4 color;

in vec2 quad_size;
in vec2 quad_position;
in float outline_width;

out vec4 f_color;

out vec2 f_position;
out vec2 f_quad_size;
out vec2 f_quad_position;
out float f_outline_width;

uniform mat4 u_projection_matrix;

void main(){
    gl_Position = u_projection_matrix * vec4(position, 1.0, 1.0);
    
    f_position = position;
    f_quad_size = quad_size;
    f_quad_position = quad_position;
    f_outline_width = outline_width;
    f_color = color;
}