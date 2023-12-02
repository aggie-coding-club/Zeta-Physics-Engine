#pragma once
#include "app.h"
#include "entity.h"
#include "shader.h"
#include "stb_image.h"

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


// NOTE(Lenny) - break this into functions
class TexturesManager{

    public:
        int textures_count = 0;
        std::vector<Texture> textures;
        
        TexturesManager(){

        }

    public:
        void add_texture(std::string path, unsigned int def_name, unsigned int format){
            textures_count++;

            Texture result = {};
            result.file_path = path;
            std::string texture_src = path;
            std::string web_texture_src = "vendor/" + texture_src;

            int width = 0;
            int height = 0;
            int nr_channels = 0;

            // Note(Lenny) : might need to be flipped
            #if __EMSCRIPTEN__
            
            unsigned char *data = stbi_load(&web_texture_src[0], &width, &height, &nr_channels, 0);
            
            #else
            
            unsigned char *data = stbi_load(&texture_src[0], &width, &height, &nr_channels, 0);
            
            #endif
            if(data){
                std::cout << "loaded png \n" << texture_src << std::endl;
            } else {
                std::cout << "failed to load png \n" << texture_src << std::endl;
            }

            glGenTextures(1, &result.id);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, result.id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            if(format == TEX_FORMAT_PNG){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
            } else if(format == TEX_FORMAT_JPG){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, data);
            } else{
                std::cout << "failed to load file type\n" << texture_src << std::endl;
                Assert(!"Failed to load texture!");
            }

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(data);

            result.def_name = def_name;
            textures.push_back(result);
        }

        void BindTexture(Texture *texture,  unsigned int slot){
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture->id);
        }
        
        void UnBindTexture(unsigned int slot){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // massive optimization to be done here
        // perhaps use a fancy search algorithm
        unsigned int GetTextureIdentifier(unsigned int def_name){
            unsigned int result = 0;
            for(int i = 0; i < textures.size(); i++){
                if(textures.at(i).def_name == def_name){
                    result = textures.at(i).id;
                    break;
                }
            }
            return result;
        }

        Texture GetTexture(unsigned int identifer){
            Texture result = {};
            for(int i = 0; i < textures.size(); i++){
                if(textures.at(i).def_name == identifer){
                    result = textures.at(i);
                    break;
                }
            }
            return result;
        }
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


void lighting_pass_render(RendererData *rd, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader);
void shadow_pass_render(RendererData *rd, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader);

void render(RendererData *rd, Camera *camera, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader);
void render_entities(RendererData *rd, Camera *camera, E_::Entity *entities[], unsigned int entity_count, TexturesManager *tm, InputManager *im);
