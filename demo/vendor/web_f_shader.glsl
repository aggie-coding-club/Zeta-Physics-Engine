#version 300 es

precision mediump float;

in vec3 f_color;
in vec2 f_tex_coords;
in vec3 f_surface_normal;
in vec3 f_to_light_vec;
in vec3 f_to_camera_vec;
in vec4 f_light_color;

out vec4 out_color;

uniform sampler2D tex_sampler;
uniform vec3 light_color;
uniform float shine_damper;
uniform float reflectivity;

void main(void){

    // out_color = vec4(color.xyz, 1);
    vec3 unit_normal = normalize(f_surface_normal);
    vec3 unit_to_light_vec = normalize(f_to_light_vec);
    
    float dot1 = dot(unit_normal, unit_to_light_vec);
    float brightness = max(dot1, 0.3);
    vec3 diffuse = brightness * light_color;

    vec3 unit_to_camera_vec = normalize(f_to_camera_vec);
    vec3 light_direction = -unit_to_light_vec;
    vec3 reflected_light_direction = reflect(light_direction, unit_normal);

    float specular_factor = dot(reflected_light_direction, unit_to_camera_vec);
    specular_factor = max(specular_factor, 0.0);
    float damped_factor = pow(specular_factor, shine_damper);

    vec3 final_specular = damped_factor *  reflectivity * light_color;

    out_color  = vec4(diffuse.xyz, 1.0) * vec4(f_color.xyz, 1.0) * texture(tex_sampler, f_tex_coords) + vec4(final_specular, 1.0f);
    // out_color  = vec4(diffuse.xyz, 1.0) * vec4(f_color.xyz, 1.0) * texture(tex_sampler, f_tex_coords) + vec4(final_specular, 1.0f);

    // out_color  = texture(tex_sampler, f_tex_coords);
    
    // out_color = vec4(dot1, 0.0, 0.0, 1.0);
    // out_color = vec4(f_color, 1.0);
    // out_color = vec4(1.0, 0.0, 0.0,1.0);
    // if(out_color.w <= 0.2){
    //     out_color = vec4(1, 1, 1, 1);
    // }
}