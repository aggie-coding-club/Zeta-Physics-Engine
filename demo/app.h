#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "HandmadeMath.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include <glad/glad.h>
#endif

#define WINDOW_WIDTH 1250
#define WINDOW_HEIGHT 980
#define DEFAULT_FOV 70

// Textures
// ================
#define TEX_FORMAT_PNG 1
#define TEX_FORMAT_JPG 2

#define TEXTURE_WHITE 1
#define TEXTURE_STALL 2
#define TEXTURE_BIRCH_LEAVES 3
#define TEXTURE_PINE_LEAVES 4
#define TEXTURE_TREE_BARK 5

// ================

struct Texture{
    std::string file_path;
    unsigned int def_name;
    unsigned int id;
};

struct RawModel{
    unsigned int vao_ID;
    unsigned int ebo_ID;
    unsigned int vbo_ID;

    unsigned int vertex_count;
};

struct VertexData{
    float *normals;
    float *positions;
    float *colors;
    float len_normals;
    float len_positions;
    float len_colors;
    int index;
    
    int *indices;
    float *tex_coords;
    float len_indices;
    float len_tex_coords;
};


struct Camera{
    HMM_Vec3 position;
    HMM_Vec3 front;
    HMM_Vec3 world_up;
    float speed;
    float pitch;
    float yaw;
    float roll;
};

struct InputManager{
    float cursorX;
    float cursorY;
    void *window;
    float dt;

    void *active_ui;
    void *hot_ui;

    unsigned int active_entity;
    unsigned int hot_entity;
    
    bool left_click;
    bool left_press;
    bool left_release;

    /** layouting
        does not allow for panels inside panels yet
    */ 
    
    HMM_Vec2 parent_pos;
};


#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}

RawModel load_to_VAO(VertexData *vertex_data);
void clean_up();

void ShowCursor(float x, float y);
void HideCursor(float x, float y);

void GameInputCamera(int key, int state);
void GameInputMouse(int button, int action);
void SetCursorPosition(float x, float y);
void SetScroll(float x_offset, float y_offset);


void PrintGLError();
void app_update(float &time_step, float dt);
void app_start(void *window);
