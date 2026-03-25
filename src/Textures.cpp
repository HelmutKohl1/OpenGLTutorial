#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image/stb_image.h"

#include "Utils.h"
#include "Shader.h"

#include <iostream>
#include <filesystem>

static const unsigned SCREEN_W = 800;
static const unsigned SCREEN_H = 600;

int textures_main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Textures", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		std::cout << "Window failed to be created" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	std::cout << "success" << std::endl;

	// initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD." << std::endl;
		return -1;
	}
	glViewport(0, 0, SCREEN_W, SCREEN_H);
	glfwSetFramebufferSizeCallback(window, framebufffer_size_callback);

	// data
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // top left
	};
	// the texture coords specify which part of the texture image to sample from at each vertex

	unsigned indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	float fraction = 0.5f;
	float increment = 0.0001f;

	// create shaders
	std::filesystem::path workingdir = std::filesystem::current_path();
	std::cout << workingdir;
	std::filesystem::path shaderSourceDir = workingdir / "res" / "textures";
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// create EBO
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex attribute of position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex attribute of colour data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// vertex attribute of texture co-ordinate data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/* 
	Texture filtering:
		 - Texture coords can be any float, so openGL has to work out which texture pixel(texel) to map the texture
		 co-ord to. The filtering can be done by nearest or using linear interpolation.
		 - You can set a mag(nifying) and a min(ifying) filter to decide how to scale up and down.
	*/

	/*
	Mipmaps: 
		- A mipmap is a collection of texture images where each subsequent texture is half the area of the previous. 
		OpenGL will switch to smaller textures in the mipmap for objects that are further away.
		To avoid harsh boundaries on the borders between mipmaps, we can filter based on using the nearest mipmap, or
		by linearlly interpolating.
	*/

	// generate texture
	GLuint texture1, texture2;
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// set the texture wrapping options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set the texture filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// load first texture resource
	stbi_set_flip_vertically_on_load(true); // flip loaded texture on y axis
	int width, height, nrChannels;
	std::filesystem::path texturePaths[2];
	texturePaths[0] = shaderSourceDir / "container.jpg";
	texturePaths[1] = shaderSourceDir / "awesomeface.png";
	unsigned char* data = stbi_load(texturePaths[0].string().c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // 2nd arg here is the mipmap level to create a texture for, base is 0
		// create mipmap (alternative is repeated calls to glTexImage2D incrementing the 2nd argument)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	// free the image data
	stbi_image_free(data);

	// bind the second texture object
	glBindTexture(GL_TEXTURE_2D, texture2);

	// set the texture wrapping options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set the texture filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load the second texture resource
	data = stbi_load(texturePaths[1].string().c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 2nd arg here is the mipmap level to create a texture for, base is 0
		// create mipmap (alternative is repeated calls to glTexImage2D incrementing the 2nd argument)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	// free the image data
	stbi_image_free(data);


	/* A texture unit is the location assigned to a texture sampler. This allows multiple 
	textures to be used in the shaders. By assigning texture units to the samplers, we can 
	bind multiple textures at once as long as we activate the corresponding texture unit first.
	Texture unit GL_TEXTURE0 is active by default, so any texture will be by default bound to it.
	*/

	shader.use();
	// tell openGL which texture unit the shader samplers belong to.
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	shader.setInt("texture2", 1); // equivalent to the above

	// render loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// detect up/down arrow presses
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			fraction += increment;
			if (fraction >= 1.0f)
				fraction = 1.0f;
			std::cout << "up\n";
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			fraction -= increment;
			if (fraction <= 0)
				fraction = 0;
			std::cout << "down\n";
		}
		shader.setFloat("fraction", fraction);

		// activate the first texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}