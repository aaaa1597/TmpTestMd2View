#include "ShaderProgram.h"
#include <android/log.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "glm/gtc/type_ptr.hpp"

using std::string;

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
//	glDeleteProgram(mHandle);
}

void ShaderProgram::SetUniform(GLuint progId, const GLchar *name, const float &f)
{
	GLint loc = GetUniformLocation(progId, name);
	glUniform1f(loc, f);
}

void ShaderProgram::SetUniform(GLuint progId, const GLchar *name, const glm::vec2 &v)
{
	GLint loc = GetUniformLocation(progId, name);
	glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::SetUniform(GLuint progId, const GLchar *name, const glm::vec3 &v)
{
	GLint loc = GetUniformLocation(progId, name);
	glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderProgram::SetUniform(GLuint progId, const GLchar *name, const glm::vec4 &v)
{
	GLint loc = GetUniformLocation(progId, name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(GLuint progId, const GLchar *name, const glm::mat4 &m)
{
	GLint loc = GetUniformLocation(progId, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

GLint ShaderProgram::GetUniformLocation(GLuint progId, const GLchar *name)
{
	auto it = mUniformLocations.find(name);

	if (it == mUniformLocations.end())
	{
		mUniformLocations[name] = glGetUniformLocation(progId, name);
	}

	return mUniformLocations[name];
}
