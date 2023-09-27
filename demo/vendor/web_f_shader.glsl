#version 300 es

precision mediump float;

in vec3 f_color;
in vec3 f_tex_coords;
in vec3 f_surface_normal;
in vec4 f_light_color;

in vec3 f_current_position;

out vec4 out_color;

uniform sampler2D tex_sampler;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;

uniform float specular_strength;
uniform float reflectivity;

void main(void){

    vec3 unit_normal = normalize(f_surface_normal);
    vec3 light_dir = normalize(light_position - f_current_position);

    float dot1 = dot(unit_normal, light_dir);
    vec3 diffuse = light_color * max(dot1, 0.3);

    vec3 camera_dir = -normalize(camera_position - f_current_position);
    vec3 reflect_dir = reflect(light_dir, unit_normal);

    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), reflectivity);
    vec3 specular = specular_strength * spec * light_color.xyz; 

    if(f_tex_coords.z == 0.0){
        out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * texture(tex_sampler, f_tex_coords.xy);

    }else if(f_tex_coords.z == 1.0){
        // out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * texture(tex_sampler, f_tex_coords.xy);
        out_color = (vec4(diffuse + specular, 1.0)) * texture(tex_sampler, f_tex_coords.xy);
        // out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * vec4(1.0, 0.0, 0.0, 1.0);
        if(texture(tex_sampler, f_tex_coords.xy).z <= 0.3){
            discard;
        }
        // out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * vec4(1.0, 1.0, 0.0, 1.0);
    } else{
        // out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * vec4(0.0, 0.0, 1.0, 1.0);
        out_color = (vec4(diffuse + specular, 1.0)) * vec4(f_color, 1.0) * texture(tex_sampler, f_tex_coords.xy);
    }
}