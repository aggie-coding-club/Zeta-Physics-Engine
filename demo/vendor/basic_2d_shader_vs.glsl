#version 300 es

// add support for texturing later
in vec2 position;
in vec4 color;

out vec4 f_color;

uniform mat4 projection_matrix;

void main(){
    gl_Position = projection_matrix * vec4(position.x, position.y, 1.0, 1.0);
    f_color = color;
}