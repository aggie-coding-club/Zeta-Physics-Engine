#version 330 core

precision mediump float;

in vec3 f_color;
in vec3 f_tex_coords;
in vec3 f_surface_normal;
in vec4 f_light_color;

in vec3 f_current_position;

out vec4 out_color;

uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;
uniform sampler2D texture_shadow_map;
uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform bool highlighted;
uniform bool selected;

uniform float specular_strength;
uniform float reflectivity;

float calculate_shadow_factor(){
    vec3 proj_coords = light_position;
    vec2 uv_coords;
    float z = 0.5 *proj_coords.z + 0.5;
    float depth = texture(texture_shadow_map, uv_coords).x;

    float bias = 0.0025;
    if(depth + bias < z){
        return 0.5;
    }else{
        return 1.0;
    }
}

void main(void){

    vec3 unit_normal = normalize(f_surface_normal);
    // vec3 light_dir = normalize(light_position - f_current_position);
    vec3 light_dir = normalize(-light_direction);

    float dot1 = dot(unit_normal, light_dir);
    vec3 diffuse = light_color * max(dot1, 0.3);

    vec3 camera_dir = -normalize(camera_position - f_current_position);
    vec3 reflect_dir = reflect(light_dir, unit_normal);

    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), reflectivity);
    vec3 specular = specular_strength * spec * light_color.xyz; 

    if (f_tex_coords.z == 0.0){
        out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * texture(texture_1, f_tex_coords.xy);
        // out_color = vec4(0.0, 0.0, 1.0, 1.0);
    } else if(f_tex_coords.z == 1.0){
        out_color = (vec4(diffuse + specular, 1.0)) * texture(texture_2, f_tex_coords.xy);
        if(texture(texture_1, f_tex_coords.xy).z <= 0.1){
            // discard;
        }
    } else if(f_tex_coords.z == 2.0){
        out_color = (vec4(diffuse + specular, 1.0)) * texture(texture_3, f_tex_coords.xy);
        if(texture(texture_1, f_tex_coords.xy).z <= 0.1){
            // discard;
        }
    } else if(f_tex_coords.z == 3.0){
        out_color = (vec4(diffuse + specular, 1.0)) * texture(texture_4, f_tex_coords.xy);
        if(texture(texture_1, f_tex_coords.xy).z <= 0.1){
            // discard;
        }
    } else {
        out_color = vec4(f_current_position.x, f_current_position.y, f_current_position.z, 1.0); // just a debug color
    }

    if(selected){
        out_color = vec4(1.0, 0.0, 0.0, 1.0);
    }
    
    if(highlighted){
        out_color = vec4(1.0, 1.0, 0.0, 1.0);
    }
}