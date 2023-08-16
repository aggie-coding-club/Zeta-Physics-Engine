#ifndef UI_H
#include "text.cpp"
#include "shader.cpp"
#include <GLFW/glfw3.h>

/** Note (Lenny) : IMGUI by Casey Muratori Notes

Traditionally, callbacks and mesages procs get messy, because data 
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

struct InputManager{
    float cursorX;
    float cursorY;
    GLFWwindow *window;

    float dt;

    void *active_ui;
    void *hot_ui;
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
    // BindLocation(&basic_2d_shader, 1, "color");

    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "projection_matrix");
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
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(4 * sizeof(float)));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(5 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(7 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    int eof = 0;
}

#define BUTTON_PADDING 5.0f;
unsigned int Button(void *id, InputManager *im, TextRendererManager *trm, String label, float xpos, float ypos, float roundness, Color color){

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
    // float width = textWidth;
    // float height = DEFAULT_TEXT_PIXEL_HEIGHT;

    float width = 250.0f;
    float height = 100.0f;

    int state = glfwGetMouseButton(im->window, GLFW_MOUSE_BUTTON_LEFT);

    if(im->cursorX >= xpos && im->cursorX <= xpos + width 
        && (WINDOW_HEIGHT - im->cursorY) >= ypos && (WINDOW_HEIGHT - im->cursorY) <= ypos + height){

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

    if(im->hot_ui == id){
        color.r = 1.0f;
        color.a = 0.5f;
    }

    if(im->active_ui == id){
        color.g = 1.0f;
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

    unsigned int u_cursor_pos = GetUniformLocation(&basic_2d_shader, "u_cursor_pos");
    SetUniformValue(u_cursor_pos, HMM_Vec2{(float)im->cursorX, WINDOW_HEIGHT - (float)im->cursorY});

    glBindVertexArray(vao2d);
    
    Color tl_color = {1.0f, 0.0f, 0.0f, 1.0f};
    Color tr_color = {1.0f, 0.0f, 0.0f, 1.0f};
    Color bl_color = {0.0f, 1.0f, 0.0f, 1.0f};
    Color br_color = {0.0f, 1.0f, 0.0f, 1.0f};

    HMM_Vec2 half_size = {width / 2.0f, height / 2.0f};
    HMM_Vec2 center = {xpos + width / 2.0f, ypos + height / 2.0f};
    float vertices[6][11] = {
        {xpos, ypos, bl_color.r, bl_color.g, bl_color.b, bl_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y},
        {xpos + width, ypos, br_color.r, br_color.g, br_color.b, br_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y},
        {xpos + width, ypos + height, tr_color.r, tr_color.g, tr_color.b, tr_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y}, 

        {xpos + width, ypos + height, tr_color.r, tr_color.g, tr_color.b, tr_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y},  
        {xpos, ypos + height, tl_color.r, tl_color.g, tl_color.b, tl_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y},
        {xpos, ypos, bl_color.r, bl_color.g, bl_color.b, bl_color.a, roundness, half_size.X, half_size.Y, center.X, center.Y}
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
    // RenderText(trm, label, scale, HMM_Vec3{115.0f, 195.0f, 55.0f}, HMM_Vec2{textXPos, textYPos});
    error = glGetError();
    return result;
}

#define UI_H
#endif