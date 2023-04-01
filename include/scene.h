#pragma once 
#include "gunslinger/gs.h"
#include <vector>

#ifdef GS_PLATFORM_WEB
    #define GS_GL_VERSION_STR "#version 300 es\n"
#else
    #define GS_GL_VERSION_STR "#version 330 core\n"
#endif


static float v_data[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static const char* v_src =
    GS_GL_VERSION_STR
    "layout(location = 0) in vec3 a_pos;\n"
    "layout(location = 1) in vec3 a_normal;\n"
    "layout(location = 2) in vec2 a_texcoord;\n"
    "uniform mat4 u_proj;\n"
    "uniform mat4 u_view;\n"
    "uniform mat4 u_model;\n"
    "out vec3 frag_pos;\n"
    "out vec3 normal;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "   frag_pos = vec3(u_model * vec4(a_pos, 1.0));\n"
    "   normal = mat3(transpose(inverse(u_model))) * a_normal;\n"
    "   tex_coord = a_texcoord;\n"
    "   gl_Position = u_proj * u_view * vec4(frag_pos, 1.0);\n"
    "}\n";

static const char* f_src =
    GS_GL_VERSION_STR
    "precision mediump float;\n"
    "layout(location = 0) out vec4 frag_color;\n"
    "in vec3 frag_pos;\n"
    "in vec3 normal;\n"
    "in vec2 tex_coord;\n"
    "struct material_t {\n"
    "   sampler2D diffuse;\n"
    "   sampler2D specular;\n"
    "   float shininess;\n"
    "};\n"
    "struct light_t {\n"
    "   vec3 position;\n"
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "   float constant;\n"
    "   float linear;\n"
    "   float quadratic;\n"
    "};\n"
    "uniform vec3 u_viewpos;\n"
    "uniform material_t u_material;\n"
    "uniform light_t u_light;\n"
    "void main() {\n"
   "    // ambient\n"
    // "   vec3 ambient = u_light.ambient * texture(u_material.diffuse, tex_coord).rgb;\n"
    "   vec3 ambient = u_light.ambient * vec3(1.0f, 1.0f, 1.0f);\n"
    "                                                                           \n" 
    "   // diffuse\n"
    "   vec3 norm = normalize(normal);\n"
    "   vec3 lightDir = normalize(u_light.position - frag_pos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = u_light.diffuse * diff * texture(u_material.diffuse, tex_coord).rgb;\n"
    "                                                                                  \n" 
    "   // specular\n"
    "   vec3 viewDir = normalize(u_viewpos - frag_pos);\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);\n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);\n"
    "   vec3 specular = u_light.specular * spec * texture(u_material.specular, tex_coord).rgb;\n"
    "                                                                                     \n" 
    "   // attenuation\n"
    "   float distance    = length(u_light.position - frag_pos);\n"
    "   float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));\n"
    "                                                                                                                  \n"
    "   ambient  *= attenuation;\n"
    "   diffuse  *= attenuation;\n"
    "   specular *= attenuation;\n"  
    "                                                                                                                  \n" 
    "   vec3 result = ambient + diffuse + specular;\n"
    // "   result = vec3(255.0f, 0.0f, 0.0f);\n"
    "   frag_color = vec4(result, 1.0);\n"
    "}\n";

struct RectPrism{
    gs_vec3 position;
    gs_vec3 scale;
    gs_vec4 color;
};

struct AppState{
    std::vector<RectPrism> rectPrisms;
};

void DrawRectPrism(AppState *appState, gs_vec3 position, gs_vec3 scale, gs_vec4 color);
void SetupScene(gs_camera_t cam);
void UpdateScene(AppState *appState, gs_camera_t cam);