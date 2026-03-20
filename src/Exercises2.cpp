#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Exercises2.h"
#include "Utils.h"
#include "Shader.h"

#include <iostream>
#include <filesystem>

int exercises2_main() {

	float vertexColours[] = {
		// positions        // colours
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};

	// GLFW set up
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "Exercises2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	std::cout << "success\n";

	// initialise GLAD so that it knows where to find the OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600); // tell openGL the viewport size
	glfwSetFramebufferSizeCallback(window, framebufffer_size_callback); // register the window resize callback with GLFW

	// create shaders
	std::filesystem::path workingdir = std::filesystem::current_path();
	std::cout << workingdir;
	std::filesystem::path shaderSourceDir = workingdir / "res" / "exercises2";
	std::filesystem::path vertexSource = shaderSourceDir / "vertex.vshader";
	std::filesystem::path fragmentSource = shaderSourceDir / "fragment.fshader";
	Shader shader = Shader(vertexSource.generic_string().c_str(), fragmentSource.generic_string().c_str());
	
	// create VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);

	// vertex attribute of position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex attribute of colour data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// apply offset
	shader.use();
	shader.setFloat("hOffset", 0.0f);
	

	while (!glfwWindowShouldClose(window)) {

		processInput(window);


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}