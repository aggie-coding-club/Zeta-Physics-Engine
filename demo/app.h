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
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define DEFAULT_FOV 70

struct RawModel{
    unsigned int vao_ID;
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

#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}

// RawModel load_to_VAO(std::vector<float> positions, 
//     std::vector<float> tex_coords, std::vector<float> normals, 
//     std::vector<int> indices, std::vector<float> color);
RawModel load_to_VAO(VertexData *vertex_data);
void clean_up();


void MoveCamera(int key, int state);
void SetCursorPosition(float x, float y);
void SetScroll(float x_offset, float y_offset);

void app_update(float &time_step, float dt);
void app_start();
