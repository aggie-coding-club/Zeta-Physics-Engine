#version 300 es

precision mediump float;

in vec3 f_color;
in vec2 f_tex_coords;
in vec3 f_surface_normal;
in vec4 f_light_color;

in vec3 f_current_position;

out vec4 out_color;

uniform sampler2D tex_sampler;
uniform vec3 light_color;
uniform vec3 light_position;

void main(void){

    vec3 unit_normal = normalize(f_surface_normal);
    vec3 light_direction = normalize(light_position - f_current_position);

    float dot1 = dot(unit_normal, light_direction);
    float diffuse = max(dot1, 0.3);

    out_color = vec4(f_color.xyz, 1.0) * texture(tex_sampler, f_tex_coords) * diffuse;
}