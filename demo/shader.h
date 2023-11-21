#pragma once
#include "app.h"

struct Shader{
    unsigned int program;
};

void enable_culling();
void disable_culling();

unsigned int get_uniform_location(Shader *shader, char *name);

void set_uniform_value(unsigned int uniform_location, HMM_Mat4 value);
void set_uniform_value(unsigned int uniform_location, HMM_Vec4 value);
void set_uniform_value(unsigned int uniform_location, HMM_Vec3 value);
void set_uniform_value(unsigned int uniform_location, HMM_Vec2 value);
void set_uniform_value(unsigned int uniform_location, float value);
void set_uniform_value(unsigned int uniform_location, int value);

void bind_location(Shader *shader, unsigned int location, char *value);

unsigned int load_shaders(std::string v_shader_path, std::string f_shader_path);