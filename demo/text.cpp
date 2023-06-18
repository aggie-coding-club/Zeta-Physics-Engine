#ifndef TEXT_H

#include "str.c"
#include "HandmadeMath.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shader.cpp"
#include <string>

#define DEFAULT_TEXT_PIXEL_HEIGHT 48

struct Character{
    unsigned int textureID;
    unsigned int advance;
    HMM_Vec2 size; // size of glyph
    HMM_Vec2 bearing; // offset from origin to top left of glyph
};

struct CharacterTable{  
    Character *characters;
    unsigned int index;
    unsigned int count; 
};

// TODO: shader program should be in here
struct TextRendererManager{
    FT_Library library;
    FT_Face *faces;
    unsigned int facesIndex;
    unsigned int facesCount;
    unsigned int vao;
    unsigned int vbo;

    CharacterTable *cts; // [""]
    unsigned int *ctsCount;

    Shader2 shader; 
    unsigned int u_projection_matrix;
    unsigned int u_text_color;
    HMM_Mat4 projection_ortho;
};

void AddFontFace(TextRendererManager *trm, const char* ttfFilePath){
    FT_Face *tempFace = trm->faces;
    FT_Face *newFace = 0;

    if(trm->facesIndex < trm->facesCount){
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[0]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &trm->faces[0];
    }else{

        int x = 0;
        trm->facesCount += 2;
        trm->faces = (FT_Face *)calloc(trm->facesCount,  sizeof(FT_Face));
        
        for(int i = 0; i < trm->facesCount - 2; i++){
            trm->faces[i] = tempFace[i];
        }
        
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[trm->facesIndex++]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &trm->faces[trm->facesIndex - 1];
    }
    FT_Set_Pixel_Sizes(newFace[0], 0, DEFAULT_TEXT_PIXEL_HEIGHT);
}

void DrawText(String string, float fontSize, HMM_Vec3 position){

}

void SetupTextRenderer(TextRendererManager *trm){
    int error = FT_Init_FreeType( &trm->library );
    if(error){
        printf("ERROR: failed to initialize freetype\n");
    }

    trm->projection_ortho = HMM_Orthographic_RH_ZO(0.0f, WINDOW_WIDTH, 0.0f, WINDOW_HEIGHT, 0.0f, 1.0f);

    trm->facesIndex = 0;
    trm->facesCount = 4;
    trm->faces = (FT_Face *)calloc(1, sizeof(FT_Face));
    // default font
    AddFontFace(trm, "Passion_One\\PassionOne-Regular.ttf");
    // AddFontFace(trm, "static\\Antonio-Bold.ttf");

    FT_Set_Pixel_Sizes(trm->faces[0], 0, 48);
    

    trm->ctsCount = (unsigned int *)(calloc(1, sizeof(unsigned int)));
    trm->ctsCount[0] = 1;

    trm->cts = (CharacterTable *)calloc(trm->ctsCount[0], sizeof(CharacterTable));
    trm->cts[0].characters = (Character *)calloc(128, sizeof(Character));
    trm->cts[0].count = 128;

    // opengl stuff
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for(unsigned char c = 0; c < 128; c++){
        // load character glyph 
        if (FT_Load_Char(trm->faces[0], c, FT_LOAD_RENDER)) {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture = 0;
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

        Character *character = &trm->cts[0].characters[c];
        trm->cts[0].index++;
        character->textureID = texture;
        character->size = {(float)trm->faces[0]->glyph->bitmap.width, (float)trm->faces[0]->glyph->bitmap.rows};
        character->bearing = {(float)trm->faces[0]->glyph->bitmap_left, (float)trm->faces[0]->glyph->bitmap_top};
        character->advance = (float)trm->faces[0]->glyph->advance.x;
        int x = 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // load text shader
    trm->shader.program = LoadShaders("text_basic_vs.glsl", "text_basic_fs.glsl");
    glUseProgram(trm->shader.program);
    BindLocation(&trm->shader, 0, "position");
    trm->u_projection_matrix = GetUniformLocation(&trm->shader, "projection_matrix");
    trm->u_text_color = GetUniformLocation(&trm->shader, "text_color");

    SetUniformValue(trm->u_projection_matrix, trm->projection_ortho);
    SetUniformValue(trm->u_text_color, HMM_Vec3{255.0f, 0.0f, 0.0f});
    
    glGenVertexArrays(1, &trm->vao);
    glGenBuffers(1, &trm->vbo);
    glBindVertexArray(trm->vao);
    glBindBuffer(GL_ARRAY_BUFFER, trm->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      
    glUseProgram(0);

    int eof = 0;
}

void RenderText(TextRendererManager *trm, String text, float scale, HMM_Vec3 color, HMM_Vec2 position){
    glUseProgram(trm->shader.program);
    SetUniformValue(trm->u_text_color, color);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(trm->vao);

    // iterate through all characters
    std::string::const_iterator c;
    for (int i = 0; i < text.length; i++){
        char c = text.val[i];
        Character ch = trm->cts[0].characters[c];

        float xpos = position.X + ch.bearing.X * scale;
        float ypos = position.Y - (ch.size.Y - ch.bearing.Y) * scale;

        float w = ch.size.X * scale;
        float h = ch.size.Y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, trm->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.X += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void CleanTextRenderer(TextRendererManager *trm){
    free(trm->faces);
    FT_Done_FreeType(trm->library);
    
    for(int i = 0; i < 1; i++){
        free(trm->cts[i].characters);
    }
    free(trm->cts);
    free(trm->ctsCount);
}

#define TEXT_H
#endif