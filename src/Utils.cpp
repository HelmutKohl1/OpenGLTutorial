#include <Utils.h>

#include <iostream>
#include <string>


void framebufffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW: error = "GL_STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW: error = "GL_STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
		//case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << "|" << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
