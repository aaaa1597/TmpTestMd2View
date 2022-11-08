#pragma once

#include <string>
#include <unordered_map>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "glm/glm.hpp"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void SetUniform(GLuint progId, const GLchar *name, const float &f);
	void SetUniform(GLuint progId, const GLchar *name, const glm::vec2 &v);
	void SetUniform(GLuint progId, const GLchar *name, const glm::vec3 &v);
	void SetUniform(GLuint progId, const GLchar *name, const glm::vec4 &v);
	void SetUniform(GLuint progId, const GLchar *name, const glm::mat4 &m);

	GLint GetUniformLocation(GLuint progId, const GLchar *name);

private:
	std::unordered_map<std::string, GLint> mUniformLocations;
};
