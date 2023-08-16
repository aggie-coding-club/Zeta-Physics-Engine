#pragma once
#include "app.h"

struct Shader{
    unsigned int program;
};

unsigned int GetUniformLocation(Shader *shader, char *name);

void SetUniformValue(unsigned int uniform_location, HMM_Mat4 value);
void SetUniformValue(unsigned int uniform_location, HMM_Vec4 value);
void SetUniformValue(unsigned int uniform_location, HMM_Vec3 value);
void SetUniformValue(unsigned int uniform_location, HMM_Vec2 value);
void SetUniformValue(unsigned int uniform_location, float value);

void BindLocation(Shader *shader, unsigned int location, char *value);

unsigned int LoadShaders(std::string v_shader_path, std::string f_shader_path);