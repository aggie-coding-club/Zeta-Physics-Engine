#ifdef __EMSCRIPTEN__
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#else
#include "glad.c"
#endif

#define CLEAR_COLOR 0.2f, 0.2f, 0.2f, 1.0f

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "app.h"

struct FileData{
    unsigned int fileSize;
    unsigned char *fileData;
};

GLFWwindow* window;

void ShowCursor(float x, float y){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPos(window, x, y);
}

void HideCursor(float x, float y){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, x, y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_D || key == GLFW_KEY_A 
        || key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_Z 
        || key == GLFW_KEY_X || key == GLFW_KEY_R || key == GLFW_KEY_ESCAPE 
        || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP
        || key == GLFW_KEY_DOWN)
            && (action == GLFW_REPEAT || action == GLFW_PRESS)){

        GameInputCamera(key, GLFW_PRESS);
    }

}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    SetCursorPosition((float)xpos, (float)ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    SetScroll((float)xoffset, (float)yoffset);
}

void RunApp(void* arg){
    static float time_step = 0.0f;
    static float dt = 1.0f;
    static float previous_time = 0.0f;
    /* Render here */
    GLFWwindow* window = *((GLFWwindow**)arg);


    glEnable(GL_DEPTH_TEST);
    glClearColor(CLEAR_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    app_update(time_step, dt);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();

    float current_time = (float)glfwGetTime();
    dt = current_time - previous_time;
    time_step += dt;
    previous_time = current_time;
}

int main(void)
{

    /* Initialize the library */
    if (!glfwInit()){
        printf("failed to init GLFW\n");
        return -1;
    }


    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Zeta Physics Engine Demo", NULL, NULL);
    if (!window)
    {
        printf("window not created\n");
        // std::cout << "Failed to create window\n" << std::endl;

        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback); 
   
    #ifdef __EMSCRIPTEN__
    #else

    // Load all OpenGL functions using the glfw loader function
    // If you use SDL you can use: https://wiki.libsdl.org/SDL_GL_GetProcAddress
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        // std::cout << "Failed to initialize OpenGL context\n" << std::endl;
        return -1;
    }else{
        // std::cout << "sucess\n" << std::endl;
    }

    #endif

    float dt = 1;
    float time_step = 0;
    app_start(window);
    float previous_time = glfwGetTime();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(RunApp, &window, 0, 1);
    #else
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glEnable(GL_DEPTH_TEST);
        glClearColor(CLEAR_COLOR);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        app_update(time_step, dt);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        float current_time = (float)glfwGetTime();
        dt = current_time - previous_time;
        time_step += dt;
        previous_time = current_time;
    }
    #endif

    clean_up();
    glfwTerminate();
    return 0;
}