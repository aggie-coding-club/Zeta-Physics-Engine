#ifndef UI_H
#include "text.cpp"
#include "shader.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "renderer.h"

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
#define BUTTON_NORMAL_COLOR {106.0f, 46.0f, 53.0f, 255.0f} // #0c0c0c
#define BUTTON_HOT_COLOR {66.0f, 46.0f, 53.0f, 255.0f} // #252424
#define BUTTON_ACTIVE_COLOR {46.0f, 66.0f, 53.0f, 255.0f} // #3e3c3c

#define BUTTON_WIDTH 220.0
#define BUTTON_HEIGHT 60.0

struct InputManager{
    float cursorX;
    float cursorY;
    GLFWwindow *window;

    float dt;

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

Shader basic_2d_shader = {};
unsigned int vao2d = 0;
unsigned int vbo2d = 0;
TexturesManager *g_tm = 0;

void DrawRectTextured(TextRendererManager *trm, HMM_Vec2 pos, float width, float height, Color color, int texture);

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

void NormalizeColor(Color *color){
    color->r = color->r / 255.0f;
    color->g = color->g / 255.0f;
    color->b = color->b / 255.0f;
    color->a = color->a / 255.0f;
}

void Setup2dRendering(TextRendererManager *trm, TexturesManager *tm){

    g_tm = tm;

    basic_2d_shader.program = load_shaders((char *)"shaders/basic_2d_shader_vs.glsl", (char *)"shaders/basic_2d_shader_fs.glsl");
    glUseProgram(basic_2d_shader.program);

    unsigned int u_projection_matrix = get_uniform_location(&basic_2d_shader, (char *)"u_projection_matrix");
    set_uniform_value(u_projection_matrix, trm->projection_ortho);

    // unsigned int u_resolution = get_uniform_location(&basic_2d_shader, (char *)"u_resolution");
    // set_uniform_value(u_resolution, HMM_Vec2{WINDOW_WIDTH, WINDOW_HEIGHT});

    glGenVertexArrays(1, &vao2d);
    glGenBuffers(1, &vbo2d);
    glBindVertexArray(vao2d);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * 6, NULL,  GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 18 * sizeof(float), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(4 * sizeof(float)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(8 * sizeof(float)));
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(10 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(12 * sizeof(float)));
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(16 * sizeof(float)));
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, 18 * sizeof(float), (GLvoid*)(17 * sizeof(float)));
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

}
void DrawRect(TextRendererManager *trm, HMM_Vec2 pos, float width, float height, Color color){
    DrawRectTextured(trm, pos, width, height, color, -1.0f);
}

void DrawRectTextured(TextRendererManager *trm, HMM_Vec2 pos, float width, float height, Color color, int texture){
    float border_width = 5.0f;
    float roundness = 5.0f;
    // draw rect
    glUseProgram(basic_2d_shader.program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glDisable(GL_DEPTH_TEST);

    unsigned int u_projection_matrix = get_uniform_location(&basic_2d_shader, (char *)"u_projection_matrix");
    set_uniform_value(u_projection_matrix, trm->projection_ortho);

    // unsigned int u_cursor_pos = get_uniform_location(&basic_2d_shader, (char *)"u_cursor_pos");
    // set_uniform_value(u_cursor_pos, HMM_Vec2{0, 0});

    glBindVertexArray(vao2d);
    
    // NormalizeColor(&color);
    Color tl_color = color;
    Color tr_color = color;
    Color bl_color = color;
    Color br_color = color;

    Color border_color = {235.0f, 235.0f, 211.0f, 255.0};

    HMM_Vec2 half_size = {width / 2.0f, height / 2.0f};
    HMM_Vec2 center = {pos.X + width / 2.0f, pos.Y + height / 2.0f};
    // NormalizeColor(&border_color);

    HMM_Vec2 tex_coords[4] = {};
    tex_coords[0] = {0.0f, 0.0f}; // bottom_left
    tex_coords[1] = {1.0f, 0.0f}; // bottom_right
    tex_coords[2] = {1.0f, 1.0f}; // top_right
    tex_coords[3] = {0.0f, 1.0f}; // top_left

    float vertices[6][18] = {
        {pos.X, pos.Y,                    tex_coords[0].X, tex_coords[0].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {pos.X + width, pos.Y,            tex_coords[1].X, tex_coords[1].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {pos.X + width, pos.Y + height,   tex_coords[2].X, tex_coords[2].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
       
        {pos.X + width, pos.Y + height,   tex_coords[2].X, tex_coords[2].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {pos.X, pos.Y + height,           tex_coords[3].X, tex_coords[3].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {pos.X, pos.Y,                    tex_coords[0].X, tex_coords[0].Y, tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    
    glActiveTexture(GL_TEXTURE0);

    if(texture == -1.0){
        glBindTexture(GL_TEXTURE_2D, g_tm->GetTextureIdentifier(TEXTURE_WHITE));
    }else{
        glBindTexture(GL_TEXTURE_2D, (unsigned int)texture);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);    
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);

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

#define BUTTON_PADDING 50.0f
unsigned int Button(void *id, InputManager *im, TextRendererManager *trm, String label, float roundness, float border_width, float xpos, float ypos, float width, float height, Color color){
    int result = 0;
    float scale = 0.4f; // Note (Lenny) : should be passed in? 
    
    String label_part_to_render = Create_String("");
    Character tallest = {};
    char c = '0';
    float text_width = 0.0f;
    for(int i = 0; i < label.length; i++){
        c = label.val[i];
        // Note (Lenny) : should have variable for active character table
        Character ch = trm->cts[0].characters[c];
        if(ch.size.Y > tallest.size.Y){
            tallest = ch;
        }

        if((text_width + (ch.advance >> 6) * scale + BUTTON_PADDING) < width){
            AddToString(&label_part_to_render, c);
            text_width += (ch.advance >> 6) * scale;
        }else{
            AddCharsToString(&label_part_to_render, "..");
            ch = trm->cts[0].characters['.'];
            text_width += (ch.advance >> 6) * scale * 2;
            break;
        }
    }
    
    HMM_Vec2 textPos = {};
    textPos.X = xpos + width / 2.0f - text_width / 2.0f;
    textPos.Y = ypos + (height / 2.0f) - ((tallest.size.Y * scale) / 2.0f);

    HMM_Vec2 pos = {xpos, ypos};
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

    // draw rect
    glUseProgram(basic_2d_shader.program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glDisable(GL_DEPTH_TEST);

    // unsigned int u_projection_matrix = get_uniform_location(&basic_2d_shader, (char *)"projection_matrix");
    // set_uniform_value(u_projection_matrix, trm->projection_ortho);

    // unsigned int u_cursor_pos = get_uniform_location(&basic_2d_shader, (char *)"u_cursor_pos");
    // set_uniform_value(u_cursor_pos, HMM_Vec2{(float)im->cursorX, WINDOW_HEIGHT - (float)im->cursorY});

    glBindVertexArray(vao2d);
    
    Color tl_color = color;
    Color tr_color = color;
    Color bl_color = color;
    Color br_color = color;

    Color border_color = {235.0f, 235.0f, 211.0f, 255.0};

    HMM_Vec2 half_size = {width / 2.0f, height / 2.0f};
    HMM_Vec2 center = {xpos + width / 2.0f, ypos + height / 2.0f};

    DrawRect(trm, pos, width, height, color);
    

#if 0
    float vertices[6][16] = {
        {xpos, ypos,                    tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {xpos + width, ypos,            tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {xpos + width, ypos + height,   tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
       
        {xpos + width, ypos + height,   tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {xpos, ypos + height,           tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
        {xpos, ypos,                    tl_color.r, tl_color.g, tl_color.b, tl_color.a,     center.X, center.Y,     half_size.X, half_size.Y,   border_color.r, border_color.g, border_color.b, border_color.a,     border_width, roundness},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);

    glDrawArrays(GL_TRIANGLES, 0, 6);    
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);

#endif
    
    RenderText(trm, label_part_to_render, scale, HMM_Vec3{255.0f / 255.0f, 231.0f / 255.0f, 147.0f / 255.0f}, HMM_Vec2{textPos.X, textPos.Y});
    DeleteString(&label_part_to_render);
    DeleteString(&label);
    
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