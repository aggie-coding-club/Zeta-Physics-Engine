#version 300 es
in vec4 position;
out vec2 tex_coords;

uniform mat4 projection_matrix;

void main(){
    gl_Position = projection_matrix * vec4(position.x, position.y, 0.0, 1.0);

    tex_coords = position.zw;
}