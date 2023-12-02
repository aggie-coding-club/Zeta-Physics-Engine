#pragma once
#include "app.h"
#include "entity.h"
#include "shader.h"
#include "textures.h"

#define PROJECTION_FOV 

struct FBO{
    unsigned int id, width, height;
};

struct ShadowMapFBO{
    FBO fbo;
    unsigned int shadow_map;
};

struct RendererData{
    HMM_Mat4 view_matrix;
    HMM_Mat4 projection_matrix;
    HMM_Mat4 shadow_view_matrix;
    HMM_Mat4 shadow_projection_matrix;

    Shader main_shader;
    Shader shadow_map_shader;
    Shader picker_shader;
    unsigned int picker_selection;
    float projection_fov;

    HMM_Vec3 main_light_pos;
    HMM_Vec3 light_dir;

    ShadowMapFBO smf;
    
    std::vector<unsigned int> vaos;
    std::vector<unsigned int> vbos;
    std::vector<unsigned int> textures;
};

HMM_Mat4 create_projection_matrix(RendererData *rd, int width, int height);
HMM_Mat4 create_view_matrix(HMM_Vec3 position, HMM_Vec3 front, HMM_Vec3 up);

void bind_fbo(FBO *fbo);
void unbind_fbo();
void unbind_VAO();
void store_data_in_attribute_list(RendererData *rd, int attribute_num, float data[], int num_of_components, int data_size);
RawModel load_to_VAO(RendererData *rd, VertexData *vertex_data);

RawModel create_cube_raw_model(RendererData *rd, ZMath::Vec3D *zeta_cube_verts);

void init_renderer(RendererData *rd);

ShadowMapFBO create_shadow_map(unsigned int width, unsigned int height);

void prepare_renderer(RendererData *rd, Camera *camera);
void prepare_shadow_renderer(RendererData *rd);


void lighting_pass_render(RendererData *rd, E_::Entity *entity, _T::TexturesManager *textures_manager, Shader *shader);
void shadow_pass_render(RendererData *rd, E_::Entity *entity, _T::TexturesManager *textures_manager, Shader *shader);

void render(RendererData *rd, Camera *camera, E_::Entity *entity, _T::TexturesManager *textures_manager, Shader *shader);
void render_entities(RendererData *rd, Camera *camera, E_::Entity *entities[], unsigned int entity_count, _T::TexturesManager *tm, InputManager *im);
