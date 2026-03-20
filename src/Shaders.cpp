#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shaders.h"
#include "Utils.h"

#include <iostream>

int shaders_main() {
	/*
	GLSL (openGL Shader Language) has the following types:
		- int, uint, float, double, bool
	Vectors can be defined as:
		vecn - default of n floats
		bvecn, ivecn, uvecn, dvecn etc.
	Components of a vector as accessed like vec.x, vec.y, or .rgba for a colour.

	Vector swizzling allows for syntax like the following:

		vec2 myvector;
		vec4 secondvec = myvector.xyxx;
	and:
		vec2 vect = vec2(0.5, 0.7);
		vec4 result = vec4(vect, 0.0, 0.0);
		vec4 result2 = vec4(result.xyz, 1.0);

	*/

	/*
	The vertex shader recieves its input straight from the vertex data. To define how the vertex data is
	organised, we specify the input variables with location metadata so we can configure the vertex attributes
	on the CPU, i.e. "layout (location = 0)".

	NB: We can also query the attribute locations in the openGL code using glGetAttribLocation().
	*/

	/*
	Uniforms are a way to pass data from the application on the CPU to the shaders on the GPU. Uniform variables
	are global, i.e. there's one per shader program and are persistent in state. They can be accessed from any shader
	within the program.
	*/

	bool triangle = true;

	float vertices[] = { // x, y, z
		// rectangle
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f // top left
	};

	float vertexColours[] = {
		// positions        // colours
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};

	unsigned indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	// source code for the vertex shader
	const char* uniformVertexShaderSource = "#version 330 core\n" // <- defines version 3.3, core profile
		"layout (location = 0) in vec3 aPos;\n" // <- define an input (using 'in') called aPos of type vec3; also set location of the input variable 
		"out vec4 vertexColour;\n" // specify a colour output to the fragment shader
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n" // <- gl_Position's value at the end of main() is the vertex shader's output.
		"   vertexColour = vec4(0.5, 0.0, 0.0, 1.0);\n"
		"}\0";

	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColour;\n"
		"out vec3 ourColour;\n"
		"void main() {\n"
		"gl_Position = vec4(aPos, 1.0f);\n"
		"ourColour = aColour;\n"
		"}\0";

	// source code for fragment shader
	const char* uniformFragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n" // <- define our output
		"uniform vec4 ourColour;\n" // <- this var is set in the application code. must be used in the GLSL or it's removed from the compiled shader code!
		"void main()\n"
		"{\n"
		"FragColor = ourColour;\n" // <- the fragment shader requires only 1 output, the colour output
		"}\0";

	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 ourColour;\n"
		"out vec4 fragColour;\n"
		"void main() {\n"
		"fragColour = vec4(ourColour, 1.0f);\n"
		"}\0";

	// GLFW set up
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	// create a vertex shader object
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader); // shader is dynamically compiled at runtime

	// check if vertex shader was successfully compiled
	int success;
	char infoLog[512];
	glad_glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// create a fragment shader object
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check if fragment shader was successfully compiled
	glad_glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// create a shader program - this is the final linked version of multiple shaders combined
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader); // attach the shaders
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // this links the output of each shader to the inpu of the next one

	// check that the linking was successful
	glad_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glad_glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// the shader objects can now be deleted once linked into a program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// create a vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// generate a vertex buffer object
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // from this point, any buffer calls (on the AR_BUF target) will be used to configure VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);

	// create an element buffer object
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute of vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// colour attribure of vertex data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// since the call the glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object, we can unbind it
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// we can also unbind VAO now, although not strictly necessary
	glBindVertexArray(0);


	// the render loop
	while (!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// clear the screen by clearing the colour buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw a triangle using just a VAO
		if (triangle) {
			// vary the colour over time using a uniform
			float timeValue = glfwGetTime();
			float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
			int vertexColourLocation = glGetUniformLocation(shaderProgram, "ourColour");

			glUseProgram(shaderProgram); // the program object can now be activated, which means it is used when rendering
			glUniform4f(vertexColourLocation, 0.0f, greenValue, 0.0f, 1.0f);
			/*
				note that finding the uniform location does not require you to first use the program, but updating the
				uniform does, since it sets the uniform on the currently active shader program.
			*/
			glBindVertexArray(VAO); // not necessary to bind VAO every time, since we only have one vertex array object
			glDrawArrays(GL_TRIANGLES, 0, 3); // 1) primative type, 2) starting index in vertex array to draw, 3) how many vertices to draw
			glBindVertexArray(0); // unbind VAO - not necessary to unbind every time
		}
		else {
			// draw a rectangle using an element buffer object
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO); // not necessary to bind VAO every time, since we only have one vertex array object
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw in wireframe mode
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); <- if using a EBO, otherwise glDrawArrays()!
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0); // unbind VAO - not necessary to unbind every time
		}

		// check and call events and swap buffers
		glfwSwapBuffers(window); // swaps between the front buffer (displayed on screen) and the back buffer (written to)
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they're no longer needed
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate(); // clean/delete GLFW's resources
	return 0;
}