#ifndef UI_H
#include "text.cpp"
#include "shader.cpp"
#include <GLFW/glfw3.h>

/** Note (Lenny) : IMGUI by Casey Muratori Notes

Traditionally, callbacks and mesages procs are get messy, because data 
 is too intermingled and wrangled.

In IMGUI, there is no state, and functions like immidiate mode graphics
 it gets rid of redundant state
  unified flow control, meaning no multiple paths to accomplish tasks. 
    no callbacks!
 in a ui element, store the minimum amount of data required to interact
  with said element
 do not have to enable, disable buttons, or make any true effort to hide
  the UI. simply just don't call the button 
 

if(do...){
    // action code
}

if / for / while{
    do(...){
    }
}

-- UI_Context struct --

struct ui_id{ // some method of identifying item
    // could just be function pointer
    int owner; int item; int index;
}

ui_id hot; // about to be interacting with item (ex: mouse hover)
ui_id active; // interacting with item (ex: mouse click)

// example implementation code
bool DoButton(UI_id, text, pos, ...) {
    if (active) {
        if (MouseWEntUp){
            if(hot) result == true;
            SetNotActive;
        } else if (hot)
            if (MouseWentDown) SetActive;

        if (inside) SetHot
    }

    // draw button
}

// to cater to introspection, use the idea of render pass and 
 layout pass or just accept one frame of lag


*/
#define BUTTON_NORMAL_COLOR {255.0f / 255.0f, 12.0f / 255.0f, 12.0f / 255.0f, 1.0f} // #0c0c0c
// #define BUTTON_NORMAL_COLOR {12.0f / 255.0f, 12.0f / 255.0f, 12.0f / 255.0f, 1.0f} // #0c0c0c
#define BUTTON_HOT_COLOR {47.0f / 255.0f, 36.0f / 255.0f, 36.0f / 255.0f, 1.0f} // #252424
#define BUTTON_ACTIVE_COLOR {82.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f} // #3e3c3c

struct InputManager{
    float cursorX;
    float cursorY;
    GLFWwindow *window;

    void *active_ui;
    void *hot_ui;

    /** layouting
        does not allow for panels inside panels yet
    */ 
    
    HMM_Vec2 parent_pos;
};

struct Color{
    union {
        struct{
            float r;
            float g;
            float b;
            float a;
        };

        float elements[4];
    };
};

Shader2 basic_2d_shader = {};
unsigned int vao2d = 0;
unsigned int vbo2d = 0;

void SetActive(InputManager *im, void *active_ui){
    im->active_ui = active_ui;
}

void SetNotActive(InputManager *im){
    im->active_ui = 0;
}

void SetHot(InputManager *im, void *hot_ui){
    im->hot_ui = hot_ui;
}

void SetNotHot(InputManager *im){
    im->hot_ui = 0;
}

void Setup2dRendering(TextRendererManager *trm){

    basic_2d_shader.program = LoadShaders("basic_2d_shader_vs.glsl", "basic_2d_shader_fs.glsl");
    glUseProgram(basic_2d_shader.program);
    BindLocation(&basic_2d_shader, 0, "position");
    BindLocation(&basic_2d_shader, 1, "color");
    BindLocation(&basic_2d_shader, 2, "quad_size");
    BindLocation(&basic_2d_shader, 3, "quad_position");
    BindLocation(&basic_2d_shader, 4, "outline_width");

    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "u_projection_matrix");
    SetUniformValue(u_projection_matrix, trm->projection_ortho);

    unsigned int u_resolution = GetUniformLocation(&basic_2d_shader, "u_resolution");
    SetUniformValue(u_resolution, HMM_Vec2{WINDOW_WIDTH, WINDOW_HEIGHT});

    glGenVertexArrays(1, &vao2d);
    glGenBuffers(1, &vbo2d);
    glBindVertexArray(vao2d);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 11 * 6, NULL,  GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(10 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    int eof = 0;
}

void DrawRect(TextRendererManager *trm, HMM_Vec2 pos, float width, float height, Color color){
    // draw rect
    glUseProgram(basic_2d_shader.program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glDisable(GL_DEPTH_TEST);

    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "u_projection_matrix");
    SetUniformValue(u_projection_matrix, trm->projection_ortho);

    glBindVertexArray(vao2d);
    
    float outline_width = 1.0f;
    HMM_Vec2 center_pos = {pos.X + width / 2.0f, pos.Y + height / 2.0f};

    float vertices[6][11] = {
        {pos.X, pos.Y, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},
        {pos.X + width, pos.Y, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},
        {pos.X + width, pos.Y + height, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},

        {pos.X + width, pos.Y + height, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},
        {pos.X, pos.Y + height, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},
        {pos.X, pos.Y, color.r, color.g, color.b, color.a, 
            width, height, center_pos.X, center_pos.Y, outline_width},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

// NOTE (Lenny) : ui will be draw relative to the bottom left of this panel
unsigned int UI_Begin(InputManager *im, HMM_Vec2 pos){

    im->parent_pos = pos;

    return true;
}

unsigned int UI_End(InputManager *im){

    im->parent_pos = {};
    return true;
}

#define BUTTON_PADDING 5.0f;
unsigned int Button(void *id, InputManager *im, TextRendererManager *trm, String label, HMM_Vec2 pos, Color color){

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

    pos += im->parent_pos;
    // float textXPos = xpos;
    HMM_Vec2 textPos = pos;
    float width = textWidth;
    float height = DEFAULT_TEXT_PIXEL_HEIGHT;

    int state = glfwGetMouseButton(im->window, GLFW_MOUSE_BUTTON_LEFT);

    if(im->cursorX >= pos.X && im->cursorX <= pos.X + width 
        && (WINDOW_HEIGHT - im->cursorY) >= pos.Y && (WINDOW_HEIGHT - im->cursorY) <= pos.Y + height){

        if(!im->active_ui){
            SetHot(im, id);
    
            if(state == GLFW_PRESS){
                SetActive(im, id);
            }

        }else{
            
            if(im->active_ui == id){
                if (state == GLFW_RELEASE){
                    SetNotActive(im);
                    result = true;
                }
            }
        }
        
    } else {
        if(state == GLFW_RELEASE){
            if(im->active_ui == id){
                SetNotActive(im);
            }

            SetNotHot(im);
        }
    }

    color = BUTTON_NORMAL_COLOR;
    if(im->hot_ui == id){
        color = BUTTON_HOT_COLOR;
    }

    if(im->active_ui == id){
        color = BUTTON_ACTIVE_COLOR;
    }

    textPos.Y = pos.Y + (height / 2.0f) - (tallest.size.Y / 2.0f);

    DrawRect(trm, pos, width, height, color);    

    unsigned int error = glGetError();
    RenderText(trm, label, scale, HMM_Vec3{255.0f / 255.0f, 231.0f / 255.0f, 147.0f / 255.0f}, HMM_Vec2{textPos.X, textPos.Y});
    error = glGetError();
    return result;
}

unsigned int Text(TextRendererManager *trm, InputManager *im, float scale, String label, HMM_Vec2 pos,  HMM_Vec3 color){
    pos += im->parent_pos;
    pos.Y += DEFAULT_TEXT_PIXEL_HEIGHT * scale;
    RenderText(trm, label, scale, color / 255.0f, pos);
    return true;
}

#define UI_H
#endif