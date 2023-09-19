#version 300 es
precision mediump float;

in vec4 f_color;
in vec2 f_world_pos;
in float f_roundness;
in vec2 f_center;
in vec2 f_half_size;

out vec4 out_color;

uniform vec2 u_resolution;
uniform vec2 u_cursor_pos;
vec2 halfSize = vec2(0.0, 0.0);
float roundness = 0.0;

void main(){
    vec3 color = vec3(f_color.x, f_color.y, f_color.z);
    vec3 border_color = vec3(1.0, 1.0, 0.0);
    
    vec2 frag_pos = gl_FragCoord.xy;

    // vec2 uv = frag_pos.xy / u_resolution;
    // uv = uv - 0.5;
    // uv = uv * u_resolution / 100.0;

    vec2 pos = abs(f_center - frag_pos);
    halfSize = f_half_size;
    roundness = f_roundness;

    halfSize -= roundness;
    float dist = length(pos - halfSize);
    bool partOf = false;

    if(pos.y > halfSize.y && pos.x < halfSize.x){
        partOf = true;
    }

    if(pos.x > halfSize.x && pos.y < halfSize.y){
        partOf = true;
    }

    if(dist < roundness){
        partOf = true;
    }

    if(pos.y < halfSize.y && pos.x < halfSize.x){
        partOf = true;
    }

    if(partOf == false){
        discard;
    }
    
    out_color = vec4(color.xyz, 1.0);
}