#include "app.h"
#include "textures.h"

namespace _T{
    
    void add_texture(TexturesManager *tm, String file_path, unsigned int def_name, unsigned int format){
        tm->textures_count++;

        Texture result = {};
        result.file_path = file_path;
        String texture_src = file_path;
        // String web_texture_src = "vendor/" + texture_src;
        String web_texture_src = "vendor/";
        web_texture_src += texture_src;
        // web_texture_src += "vendor/";
        // AddCharsToString("vendor/", &texture_src);
        int x = 0;

#if 1
        int width = 0;
        int height = 0;
        int nr_channels = 0;

        // Note(Lenny) : texture might need to be flipped
        #if __EMSCRIPTEN__
        unsigned char *data = stbi_load(web_texture_src.val, &width, &height, &nr_channels, 0);

        #else
        
        unsigned char *data = stbi_load(texture_src.val, &width, &height, &nr_channels, 0);
        
        #endif
        if(data){
            printf("loaded png : %s\n", texture_src.val);
        } else {
            printf("failed to load png : \n", texture_src.val);
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
            printf("failed to load file type : %s\n", texture_src.val);
            Assert(!"Failed to load texture!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        result.def_name = def_name;
        tm->textures[tm->textures_count] = result;
#endif
    }
    
    void bind_texture(unsigned int texture, unsigned int slot){
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void unbind_texture(unsigned int slot){
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int GetTextureIdentifier(TexturesManager *tm, unsigned int def_name){
        unsigned int result = 0;

        for(int i = 0; i < tm->textures_count + 1; i++){
            if(tm->textures[i].def_name == def_name){
                result = tm->textures[i].id;
                break;
            }
        }

        return result;
    }

    Texture GetTexture(TexturesManager *tm, unsigned int identifer){
        Texture result = {};

        for(int i = 0; i < tm->textures_count; i++){
            if(tm->textures[i].def_name == identifer){
                result = tm->textures[i];
                break;
            }
        }

        return result;
    }

};