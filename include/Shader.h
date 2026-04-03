#pragma once

#ifndef SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	// the program ID
	unsigned ID;

	// constructor reads and builds the shader
	//Shader(const char *vertexPath, const char* fragmentPath);
	// use/activate the shader
	//void use();
	// utility uniform functions
	//void setBool(const std::string& name, bool value) const;
	//void setInt(const std::string& name, int value) const;
	//void setFloat(const std::string& name, float value) const;

	Shader(const char *vertexPath, const char *fragmentPath) {
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// convert streams into strings
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		GLuint vertex, fragment;
		int success;
		char infoLog[512];

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompilationLinking(vertex, "VERTEX");

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompilationLinking(fragment, "FRAGMENT");

		// shader program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompilationLinking(ID, "PROGRAM");

		// delete the shaders as they're linked into the program and so no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use() {
		glUseProgram(ID);
	}

	void setBool(std::string name, bool value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform1i(location, value);
	}

	void setInt(std::string name, bool value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform1i(location, value);
	}

	void setFloat(std::string name, bool value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform1f(location, value);
	}

	void setVec2(std::string name, const glm::vec2& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform2fv(location, 1, &value[0]);
	}

	void setVec3(std::string name, const glm::vec3& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3fv(location, 1, &value[0]);
	}

	void setVec4(std::string name, const glm::vec4& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform4fv(location, 1, &value[0]);
	}

	void setMat2(std::string name, const glm::mat2& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
	}

	void setMat3(std::string name, const glm::mat3& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
	}

	void setMat4(std::string name, const glm::mat4& value) const {
		int location = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	private:
		void checkCompilationLinking(GLuint shader, std::string type) {
			int success = 0;
			char infoLog[512];
			if (type != "PROGRAM") {
				// check compilation
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(shader, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
				}
			}
			else {
				// check linking
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(ID, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
				}
			}
		}
}; 

#endif // !SHADER_H

