
#ifndef GAME_SYSTEM
#define GAME_SYSTEM

#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

//define in main.cpp
extern GLFWwindow* window;

// "Initialize GLFW, GLEW, Input, Create window
int SystemInit(int width, int height, const char* title);

void SystemShutdown();

// Get frame duration between each frame
void FrameInit();
double FrameStart();
void FrameEnd();


#endif // GAME_SYSTEM