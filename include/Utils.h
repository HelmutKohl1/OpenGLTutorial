#pragma once

#include <GLFW/glfw3.h>

#define glCheckError() glCheckError_(__FILE__, __LINE__);

void framebufffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLenum glCheckError_(const char* file, int line);