#include "str.c"
#include "HandmadeMath.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct TextRendererManager{
    FT_Library library;
};

void SetupTextRenderer(TextRendererManager *trm){
    int error = FT_Init_FreeType( &trm->library );
    if(error){
        printf("ERROR: failed to initialize freetype\n");
    }

    
}

void DrawText(String string, float fontSize, HMM_Vec3 position){

}

void RenderText(float dt){

}

void CleanTextRenderer(){

}