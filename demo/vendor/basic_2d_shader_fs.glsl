#version 300 es
precision mediump float;

in vec4 f_color;
in vec2 f_world_pos;

in vec2 f_half_size;
in vec2 f_center;

in vec4 f_border_color;
in float f_border_width;
in float f_roundness;

out vec4 out_color;

uniform vec2 u_resolution;
uniform vec2 u_cursor_pos;
// vec2 halfSize = vec2(0.0, 0.0);
float roundness = 0.0;

vec2 halfSize = vec2(125.0, 50.0);

void main(){
    vec3 color = f_border_color.xyz;
    
    vec2 frag_pos = gl_FragCoord.xy;

    vec2 pos = abs(f_center - frag_pos);
    bool partOf = false;

if(true == true){
    halfSize = f_half_size;
    halfSize -= f_roundness + f_border_width;
    float dist = length(pos - halfSize);
    bool partOf = false;

    if((pos.y - halfSize.y) < f_roundness && (pos.x - halfSize.x) < f_roundness){

        if(pos.x > halfSize.x && pos.y > halfSize.y){

            if(dist < f_roundness){
                partOf = true;
            }
        }else{
            partOf = true;
        }
    }

    if(partOf == true){
        color = vec3(f_color.x, f_color.y, f_color.z);
    }

    partOf = false;
}

if(true == true){ 
    halfSize = f_half_size;
    halfSize -= f_roundness;
    float dist = length(pos - halfSize);

    if(pos.y > halfSize.y && pos.x < halfSize.x){
        partOf = true;
    }

    if(pos.x > halfSize.x && pos.y < halfSize.y){
        partOf = true;
    }

    if(dist < f_roundness){
        partOf = true;
    }

    if(pos.y < halfSize.y && pos.x < halfSize.x){
        partOf = true;
    }

    if(partOf == false){
        discard;
    }
}
    
    out_color = vec4(color.xyz, 1.0);
}