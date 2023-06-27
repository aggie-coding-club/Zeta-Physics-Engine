#version 300 es
precision mediump float;

in vec4 f_color;

in vec2 f_position;
in vec2 f_quad_size;
in vec2 f_quad_position;
in float f_outline_width;

vec3 color;
out vec4 out_color;
uniform vec2 u_resolution;

// https://iquilezles.org/articles/distfunctions2d/
float sdf_circle(vec2 p, float r){
    return length(p) - r;
}

void main(){
    // vec2 pos = gl_FragCoord.xy;
    // pos.x -= f_quad_position.x;
    // pos.y -= f_quad_position.y;
    
    // vec2 uv = (pos.xy) / (u_resolution / 2.0); 
    // uv = uv - 0.5;
    // uv = uv * u_resolution / 100.0;

    // vec3 black = vec3(0.0);
    // vec3 white = vec3(1.0);
    // vec3 red = vec3(1.0, 0.0, 0.0);
    // vec3 blue = vec3(0.65, 0.85, 1.0);
    // vec3 orange = vec3(0.9, 0.6, 0.2);

    // drawing circle sdf
    // float radius = 1.0;
    // vec2 center = vec2(0.0, 0.0);
    // float distance_to_circle = sdf_circle(uv - center, radius);
    // color = distance_to_circle > 0.0 ? orange : blue;

    out_color = vec4(f_color.xyz, 1.0);
    // color = f_color.xyz;
    // color.x = uv.x;
    // color.y =  uv.y;
    // color.z = 0.0;
    // color = vec4(f_outline_width, 0.0, 0.0, 1.0);
}