#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc\type_ptr.hpp>

class Shader
{
private:
	GLuint id;
	const int versionMajor;
	const int versionMinor;

	std::string loadShaderSource(char* fileName);

	GLuint loadShader(GLenum type, char* fileName);

	void linkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);

public:
	Shader(
		const int versionMajor,
		const int versionMinor,
		const char* vertexFile = "", 
		const char* fragmentFile = "", 
		const char* geometryFile = "") : versionMajor(versionMajor), versionMinor(versionMinor)
	{
		GLuint vertexShader = 0;
		GLuint geometryShader = 0;
		GLuint fragmentShader = 0;

		vertexShader = loadShader(GL_VERTEX_SHADER, (char*)vertexFile);

		if (geometryFile != "")
		{
			geometryShader = loadShader(GL_GEOMETRY_SHADER, (char*)geometryFile);
		}

		fragmentShader = loadShader(GL_FRAGMENT_SHADER, (char*)fragmentFile);

		this->linkProgram(vertexShader, geometryShader, fragmentShader);

		//End
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);
	}

	~Shader()
	{
		glDeleteProgram(this->id);
	}

	inline GLuint getProgramId() { return id; }

	void use() { glUseProgram(this->id); }
	void unuse() { glUseProgram(0); }

	void set1i(GLint value, const GLchar* name);
	void set1f(GLfloat value, const GLchar* name);
	void setVec2f(glm::fvec2 value, const GLchar* name);
	void setVec3f(glm::fvec3 value, const GLchar* name);
	void setVec4f(glm::fvec4 value, const GLchar* name);
	void setMat3fv(glm::mat3 value, const GLchar* name, GLboolean transpose = GL_FALSE);
	void setMat4fv(glm::mat4 value, const GLchar* name, GLboolean transpose = GL_FALSE);
};