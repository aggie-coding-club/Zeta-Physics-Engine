#version 300 es

// add support for texturing later
in vec2 position;
in vec4 color;
in float roundness;
in vec2 half_size;
in vec2 center;

out vec4 f_color;
out vec2 f_world_pos;

out float f_roundness;
out vec2 f_half_size;
out vec2 f_center;

uniform mat4 projection_matrix;

void main(){
    gl_Position = projection_matrix * vec4(position.x, position.y, 1.0, 1.0);
    f_world_pos = gl_Position.xy;
    f_color = color;

    f_roundness = roundness;
    f_half_size = half_size;
    f_center = center;
}