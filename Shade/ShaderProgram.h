#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "GL\glew.h"
#include "Shader.h"
#include <vector>

class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
	~ShaderProgram();

	void addShader(const Shader& shader);
	void link(bool shouldDeleteShaders = true);

	const GLuint getID() const;
private:
	GLuint id;
	std::vector<GLuint> shaderIds;
};

#endif
