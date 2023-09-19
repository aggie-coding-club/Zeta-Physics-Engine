#version 300 es

// add support for texturing later
in vec2 position;
in vec4 color;

in vec2 center;
in vec2 half_size;

in vec4 border_color;
in float border_width;
in float roundness;


////////////////////////
out vec4 f_color;
out vec2 f_world_pos;

out vec2 f_half_size;
out vec2 f_center;

out vec4 f_border_color;
out float f_border_width;
out float f_roundness;

uniform mat4 u_projection_matrix;

void main(){
    gl_Position = u_projection_matrix * vec4(position.x, position.y, 1.0, 1.0);
    f_world_pos = gl_Position.xy;
    
    f_color = color;

    f_center = center;
    f_half_size = half_size;
    
    f_border_color = border_color;
    f_border_width = border_width;
    f_roundness = roundness;
}