#include "str.c"
#include "HandmadeMath.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct TextRendererManager{
    FT_Library library;
    FT_Face *faces;
    unsigned int facesIndex;
    unsigned int facesCount;
};

void AddFontFace(TextRendererManager *trm, const char* ttfFilePath){
    FT_Face *tempFace = trm->faces;

    if(trm->facesIndex < trm->facesCount){
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[trm->facesIndex++]);
        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }
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

    int i = 0;
}

void RenderText(float dt){

}

void CleanTextRenderer(TextRendererManager *trm){
    free(trm->faces);
}