#include "str.c"
#include "HandmadeMath.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character{
    unsigned int textureID;
    unsigned int advance;
    HMM_Vec2 size; // size of glyph
    HMM_Vec2 brearing; // offset from origin to top left of glyph
};

struct CharacterTable{  
    Character *characters;
    unsigned int index;
    unsigned int count; 
};

struct TextRendererManager{
    FT_Library library;
    FT_Face *faces;
    unsigned int facesIndex;
    unsigned int facesCount;

    CharacterTable *cts; // [""]
    unsigned int *ctsCount;
};

void AddFontFace(TextRendererManager *trm, const char* ttfFilePath){
    FT_Face *tempFace = trm->faces;
    FT_Face *newFace = 0;

    if(trm->facesIndex < trm->facesCount){
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[trm->facesIndex++]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &trm->faces[trm->facesIndex - 1];
    }else{
        trm->facesCount += 2;
        trm->faces = (FT_Face *)calloc(sizeof(FT_Face), trm->facesCount);
        
        for(int i = 0; i < trm->facesCount - 2; i++){
            trm->faces[i] = tempFace[i];
        }
        
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[trm->facesIndex++]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &trm->faces[trm->facesIndex - 1];
    }

    FT_Set_Pixel_Sizes(*newFace, 0, 48);

    if (FT_Load_Char(*newFace, 'X', FT_LOAD_RENDER)){
        printf("ERROR: failed to load Glyph\n");  
    }else{
        printf("loaded glyph!!\n");
    }
}

void DrawText(String string, float fontSize, HMM_Vec3 position){

}

void SetupTextRenderer(TextRendererManager *trm){
    int error = FT_Init_FreeType( &trm->library );
    if(error){
        printf("ERROR: failed to initialize freetype\n");
    }

    trm->facesIndex = 0;
    trm->facesCount = 4;
    trm->faces = (FT_Face *)calloc(sizeof(FT_Face), 4);
    // default font
    AddFontFace(trm, "Passion_One\\PassionOne-Regular.ttf");

    trm->ctsCount = (unsigned int *)(calloc(1, sizeof(unsigned int)));
    trm->ctsCount[0] = 1;

    trm->cts = (CharacterTable *)calloc(trm->ctsCount[0], sizeof(CharacterTable*));
    trm->cts[0].characters = (Character *)calloc(128, sizeof(Character));
    trm->cts[0].count = 128;
    trm->cts[0].index = 0;

    // opengl stuff
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for(unsigned char c = 0; c < trm->cts[0].count; c++){
        // load character glyph 
        if (FT_Load_Char(trm->faces[0], c, FT_LOAD_RENDER)) {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            trm->faces[0]->glyph->bitmap.width,
            trm->faces[0]->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            trm->faces[0]->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character *character = &trm->cts[0].characters[trm->cts[0].index++];
        character->textureID = texture;
        character->size = {(float)trm->faces[0]->glyph->bitmap.width, (float)trm->faces[0]->glyph->bitmap.rows};
        character->brearing = {(float)trm->faces[0]->glyph->bitmap_left, (float)trm->faces[0]->glyph->bitmap_top};
        character->advance = (float)trm->faces[0]->glyph->advance.x;

        int x = 0;
    }
    int eof = 0;
}

void RenderText(float dt){

}

void CleanTextRenderer(TextRendererManager *trm){
    free(trm->faces);
    FT_Done_Face(trm->faces[0]);
    FT_Done_FreeType(trm->library);

    free(trm->faces);
    
    for(int i = 0; i < 1; i++){
        free(trm->cts[i].characters);
    }
    free(trm->cts);
    free(trm->ctsCount);
}