#pragma once
#include "str.h"
#include "stb_image.h"

#define MAX_TEXTURES 64
namespace _T{
    
    struct Texture{
        String file_path;
        unsigned int def_name;
        unsigned int id;
    };

    struct TexturesManager{
        int textures_count;
        Texture textures[MAX_TEXTURES];
    };

    void add_texture(TexturesManager *tm, String file_path, unsigned int def_name, unsigned int format);
    void bind_texture(Texture *texture, unsigned int slot);
    void unbind_texture(unsigned int slot);
    unsigned int GetTextureIdentifier(TexturesManager *tm, unsigned int def_name);
    Texture GetTexture(TexturesManager *tm, unsigned int identifer);

};