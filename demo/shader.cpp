#include "shader.h"

unsigned int GetUniformLocation(Shader *shader, char *name){
    unsigned int result = 0;
    result = glGetUniformLocation(shader->program, name);
    if(result == -1){
        // printf("Failed to Get Uniform Location -> %s \n ", name);
        // printf("failed to get uniform location\n");
        // Assert(!"Failed to Get Uniform Location");
    }
    return result;
}


void EnableCulling(){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void DisableCulling(){
    glDisable(GL_CULL_FACE);
}

// must have a shader bound to have set uniform value
void SetUniformValue(unsigned int uniform_location, HMM_Mat4 value){
    glUniformMatrix4fv(uniform_location, 1, false, &value[0][0]);
}

void SetUniformValue(unsigned int uniform_location, HMM_Vec4 value){
    glUniform4f(uniform_location, value.X, value.Y, value.Z, value.W);
}

void SetUniformValue(unsigned int uniform_location, HMM_Vec3 value){
    glUniform3f(uniform_location, value.X, value.Y, value.Z);
}

void SetUniformValue(unsigned int uniform_location, HMM_Vec2 value){
    glUniform2f(uniform_location, value.X, value.Y);
}

void SetUniformValue(unsigned int uniform_location, float value){
        glUniform1f(uniform_location, value);
}

void SetUniformValue(unsigned int uniform_location, int value){
        glUniform1i(uniform_location, value);
}

void BindLocation(Shader *shader, unsigned int location, char *value){
    glBindAttribLocation(shader->program, location, value);
}

unsigned int LoadShader(GLenum shaderType, std::string path){

    #ifdef __EMSCRIPTEN__
    std::fstream shader_file("vendor/" + path);
    #else
    std::fstream shader_file(path);
    #endif

    if(!shader_file){
        std::cout << "Error loading shader file -- " << path << std::endl;
    }else{
        std::cout << "Successfully loaded shader " << path  << std::endl;
    }

    std::string shader_src;

    std::string temp_line;
    while(std::getline(shader_file, temp_line)){
        shader_src += temp_line + "\n";
    }

    unsigned int result = glCreateShader(shaderType);

    const char *shader_src_address =  &shader_src[0];
    glShaderSource(result, 1, &shader_src_address, 0);
    glCompileShader(result);

    int shader_compiled;
    glGetShaderiv(result, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(result, 1024, &log_length, message);
        // Write the error to a log
        std::cout << (message)  << std::endl;
    }
    
    shader_file.close(); 

    return result;
}

unsigned int LoadShaders(std::string v_shader_path, std::string f_shader_path){
    unsigned int v_shader = LoadShader(GL_VERTEX_SHADER, v_shader_path);
    unsigned int f_shader = LoadShader(GL_FRAGMENT_SHADER, f_shader_path);

    // shader program
    unsigned int result = glCreateProgram();
    glAttachShader(result, v_shader);
    glAttachShader(result, f_shader);

    glLinkProgram(result);

    int program_linked = 0;
    glGetProgramiv(result, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(result, 1024, &log_length, message);
        // Write the error to a log
    }

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    
    glUseProgram(0);

    return result;
}
