#include "Shader.h"

#include <fstream>
#include <streambuf>
#include "../Constants.h"

Shader::Shader()
{
}

Shader::Shader(const std::string & name, GLuint type)
{
	create(name, type);
	compile();
}

Shader::~Shader()
{
	glDeleteShader(this->id);
}

void Shader::create(const std::string & name, GLuint type)
{
	this->id = glCreateShader(type);
	std::ifstream file(SHADER_MAP_PATH + name);
	std::string shaderText;
	if (file.is_open())
	{
		shaderText = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	}
	else
	{
		fprintf(stderr, "Could not open shader: %s\n", SHADER_MAP_PATH + name);
	}
	const char* shaderTextPtr = shaderText.c_str();
	glShaderSource(this->id, 1, &shaderTextPtr, nullptr);
}

void Shader::compile()
{
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = GL_FALSE;
	glCompileShader(this->id);
	glGetShaderiv(this->id, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(this->id, 1024, nullptr, buff);
		fprintf(stderr, buff);
	}
}

const GLuint Shader::getID() const
{
	return this->id;
}
