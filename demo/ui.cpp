#ifndef UI_H
#include "text.cpp"
#include "shader.cpp"

#if 0 
// Note ( Lenny ) - good idea?
struct UI_attribs{
    float xPos;
    float yPos;
    float width;
    float height;
};
#endif

struct InputManager{
    float cursorX;
    float cursorY;
};

Shader2 basic_2d_shader = {};
unsigned int vao2d = 0;
unsigned int vbo2d = 0;

void Setup2dRendering(TextRendererManager *trm){

    basic_2d_shader.program = LoadShaders("basic_2d_shader_vs.glsl", "basic_2d_shader_fs.glsl");
    glUseProgram(basic_2d_shader.program);
    BindLocation(&basic_2d_shader, 0, "position");
    // BindLocation(&basic_2d_shader, 1, "color");

    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "projection_matrix");
    SetUniformValue(u_projection_matrix, trm->projection_ortho);

    glGenVertexArrays(1, &vao2d);
    glGenBuffers(1, &vbo2d);
    glBindVertexArray(vao2d);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, NULL,  GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    int eof = 0;
}


struct Color{
    float r;
    float g;
    float b;
    float a;
};

#define BUTTON_PADDING 5.0f;
unsigned int Button(void *id, InputManager *im, TextRendererManager *trm, String label, float xpos, float ypos, Color color){

    int result = 0;
    float scale = 1.0f; // should be passed in 
    
    Character tallest = {};
    char c = '0';
    float textWidth = 0.0f;
    for(int i = 0; i < label.length; i++){
        c = label.val[i];
        // Note (Lenny) : should have variable for active character table
        Character ch = trm->cts[0].characters[c];
        if(ch.size.Y > tallest.size.Y){
            tallest = ch;
        }

        textWidth += (ch.advance >> 6) * scale;
    }

    float textXPos = xpos;
    float width = textWidth;
    float height = DEFAULT_TEXT_PIXEL_HEIGHT;

    if(im->cursorX >= xpos && im->cursorX <= xpos + width 
        && (WINDOW_HEIGHT - im->cursorY) >= ypos && (WINDOW_HEIGHT - im->cursorY) <= ypos + height){
        color.a = 0.5f;
    }

    float textYPos = ypos + (height / 2.0f) - (tallest.size.Y / 2.0f);

    // draw rect
    glUseProgram(basic_2d_shader.program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glDisable(GL_DEPTH_TEST);

    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "projection_matrix");
    SetUniformValue(u_projection_matrix, trm->projection_ortho);

    glBindVertexArray(vao2d);
    
    float vertices[6][6] = {
        {xpos, ypos, color.r, color.g, color.b, color.a},
        {xpos + width, ypos, color.r, color.g, color.b, color.a},
        {xpos + width, ypos + height, color.r, color.g, color.b, color.a},

        {xpos + width, ypos + height, color.r, color.g, color.b, color.a},
        {xpos, ypos + height, color.r, color.g, color.b, color.a},
        {xpos, ypos, color.r, color.g, color.b, color.a},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
    
    unsigned int error = glGetError();
    RenderText(trm, label, scale, HMM_Vec3{115.0f, 195.0f, 55.0f}, HMM_Vec2{textXPos, textYPos});
    error = glGetError();
    return result;
}

#define UI_H
#endif