#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Test.h"
#include "Utils.h"
#include <iostream>


static bool triangle = false;

static float vertices[] = { // x, y, z
	// rectangle
	0.5f, 0.5f, 0.0f, // top right
	0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f, // bottom left
	-0.5f, 0.5f, 0.0f // top left
};

static unsigned indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

// source code for the vertex shader
static const char *vertexShaderSource = "#version 330 core\n" // <- defines version 3.3, core profile
"layout (location = 0) in vec3 aPos;\n" // <- define an input (using 'in') called aPos of type vec3; also set location of the input variable 
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n" // <- gl_Position's value at the end of main() is the vertex shader's output.
"}\0";

// source code for fragment shader
static const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n" // <- define our output
"\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // <- the fragment shader requires only 1 output, the colour output
"}\0";

int test_main() {
	
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// create an element buffer object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/*
	* The usage param determines how the gpu should manage the data given to the buffer, it can be:
	*  - GL_STREAM_DRAW: The data is set only once and used by the GPU at most a few times
	*  - GL_STATIC_DRAW: The data is set only once and used many times
	*  - GL_DYNAMIC_DRAW: The data is changed a lot and used many times
	*/	

	/*
		We need to tell openGL how to interpret the vertex data in memory, and how it should connect the vertex data
		to the vertex shader's attributes.

		Inputs to the vertex shader are specified in terms of vertex attributes, we need to specify how the input data
		connects to the vertex attributes in the shader, i.e. to specify how to interpret the vertex data before rendering.
		The vertex buffer data is formatted as:

		|  vertex1  |  vertex2  |  vertex3  |
		| x | y | z | x | y | z | x | y | z |
byte:	0   4   8   12  16  20  24  28  32  36

		The position data is stored as 32 bit floats, each position being 3 of these.
		There are no spaces between them, and the first value is at the beginning of the buffer.

		Knowing this, we can tell openGL how to interpret the vertex data (per vertex attribute) using the following call:
		Params are:
			1) which vertex attribute to configure, this was set as 0 with 'layout (location = 0)' in the vertex shader source.
			2) size of the vertex attribute, 3 since it's a vec3.
			3) type of data, which since it's a vec3 is GL_FLOAT
			4) if we want the data to be normalised. No, since we've already done this
			5) the stride, i.e. the space between consecutive vertex attributes.
			6) the offset of where the position data begins within the buffer

		Each vertex attribute takes its data from memory managed by a VBO, which VBO is determined by the VBO currently bound to
		GL_ARRAY_BUFFER when calling glVertexAttribPointer.
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// since the call the glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object, we can unbind it
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// we can also unbind VAO now, although not strictly necessary
	glBindVertexArray(0);

	/* The process to draw an object in openGL is now like:
	
		0. copy the vertices array into a buffer for openGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		1. set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		2. use our shader program when we want to render an object
		glEnableProgram(shaderProgram);

		3. now draw the object
		someOpenGLFunctionThatDrawsTheObject();

	Using a VAO, this becomes:
		0. initialisation code
		1. bind vertex array object
		glBindVertexArray(VAO);

		2. copy the vertixes array in a buffer for openGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		3. set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		4. drawing code inside render loop
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		someOpenGLFunctionThatDrawsTheObject();
	*/

	/*
		A vertex array object stores the following:
		 - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
		 - vertex attribute configurations via glVertexAttribPointer
		 - vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer
	*/

	// to use a VAO, you just need to bind it using glBindVertexArray. We then bind/configure the corresponding VBOs and attrib pointers,
	// then unbind for later use. To draw an object, we simply bind the VAO with the preferred drawing settings.


	// the render loop
	while (!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// clear the screen by clearing the colour buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw a triangle using just a VAO
		if (triangle) {
			glUseProgram(shaderProgram); // the program object can now be activated, which means it is used when rendering
			glBindVertexArray(VAO); // not necessary to bind VAO every time, since we only have one vertex array object
			glDrawArrays(GL_TRIANGLES, 0, 3); // 1) primative type, 2) starting index in vertex array to draw, 3) how many vertices to draw
			glBindVertexArray(0); // unbind VAO - not necessary to unbind every time
		}
		else {
		// draw a rectangle using an element buffer object
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO); // not necessary to bind VAO every time, since we only have one vertex array object
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw in wireframe mode
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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