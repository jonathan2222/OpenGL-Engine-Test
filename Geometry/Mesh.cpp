#include "Mesh.h"
#include <stdio.h>

#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	delete this->material;
	glDeleteBuffers(1, &(this->vertexBufferID));
	glDeleteBuffers(1, &(this->indexBufferID));
	glDeleteVertexArrays(1, &(this->vaoID));
}

void Mesh::create(const GLuint shaderProgramID)
{
	glGenVertexArrays(1, &(this->vaoID)); // Stores one or more vbo (Vertex buffer object)
	glBindVertexArray(this->vaoID);

	glGenBuffers(1, &(this->vertexBufferID)); // Create a vbo which holds the position, normals and uvs. This can be divided into 3 separate vbos if needed.
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

	glUseProgram(shaderProgramID);

	// Position
	GLint vertexPosID = glGetAttribLocation(shaderProgramID, "vertexPosition");
	if (vertexPosID == -1) { fprintf(stderr, "Error, cannot find 'vertexPosition' attribute in Vertex shader\n"); return; }
	glEnableVertexAttribArray(vertexPosID); // This is now affecting the vaoID which was created. 
	glVertexAttribPointer(vertexPosID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)nullptr);

	// Noraml
	GLuint vertexNormalID = glGetAttribLocation(shaderProgramID, "vertexNormal");
	if (vertexNormalID == -1) { fprintf(stderr, "Error, cannot find 'vertexNormal' attribute in Vertex shader\n"); return; }
	glEnableVertexAttribArray(vertexNormalID);
	glVertexAttribPointer(vertexNormalID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(sizeof(GLfloat)*3));

	// Texture coords
	GLuint vertexUvsID = glGetAttribLocation(shaderProgramID, "vertexUvs");
	if (vertexUvsID == -1) { fprintf(stderr, "Error, cannot find 'vertexUvs' attribute in Vertex shader\n"); return; }
	glEnableVertexAttribArray(vertexUvsID);
	glVertexAttribPointer(vertexUvsID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(sizeof(GLfloat) * 6));

	// Indices
	glGenBuffers(1, &(this->indexBufferID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

	// World and Model matrices
	worldMat = glGetUniformLocation(shaderProgramID, "world");
	if (worldMat == -1) fprintf(stderr, "Can't find 'worldMat' in the shader!");
	world = glm::mat4(1.0f);
	glUniformMatrix4fv(worldMat, 1, GL_FALSE, &world[0][0]);

	modelMat = glGetUniformLocation(shaderProgramID, "model");
	if (modelMat == -1) fprintf(stderr, "Can't find 'modelMat' in the shader!");
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMat, 1, GL_FALSE, &model[0][0]);

	// Add material values to the GPU
	for (int i = 0; i < 3; i++)
	{
		materialData.ka[i] = material->ka[i];
		materialData.kd[i] = material->kd[i];
		materialData.ks[i] = material->ks[i];
	}
	materialData.ks[3] = (int)material->ns;

	GLuint bindingPoint = 1;
	GLuint uboIndex = glGetUniformBlockIndex(shaderProgramID, "MaterialData");
	glUniformBlockBinding(shaderProgramID, uboIndex, bindingPoint);

	glUseProgram(0);

	glGenBuffers(1, &UBOMaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMaterial);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialData), &materialData, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBOMaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::draw() const
{
	// tell opengl we are going to use the VAO we described earlier
	glBindVertexArray(this->vaoID);

	//glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::rotate(float angle, const glm::vec3 & axis)
{
	this->model = glm::rotate(this->model, angle, axis);
	glUniformMatrix4fv(worldMat, 1, GL_FALSE, &this->model[0][0]);
}

void Mesh::createBoundingBox()
{
	this->maxPoint = {0.0f, 0.0f, 0.0f};
	this->minPoint = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < this->vertices.size(); i++)
	{
		glm::vec3 pos = glm::make_vec3(this->vertices[i].position);
		if (i != 0)
		{
			this->maxPoint.x = glm::max(maxPoint.x, pos.x);
			this->maxPoint.y = glm::max(maxPoint.y, pos.y);
			this->maxPoint.z = glm::max(maxPoint.z, pos.z);
			
			this->minPoint.x = glm::min(minPoint.x, pos.x);
			this->minPoint.y = glm::min(minPoint.y, pos.y);
			this->minPoint.z = glm::min(minPoint.z, pos.z);
		}
		else
		{
			this->maxPoint = pos;
			this->minPoint = pos;
		}
	}
}

float Mesh::getWidth() const
{
	return this->maxPoint.x-this->minPoint.x;
}

float Mesh::getHeight() const
{
	return this->maxPoint.y - this->minPoint.y;
}

float Mesh::getDepth() const
{
	return this->maxPoint.z - this->minPoint.z;
}

glm::vec3 Mesh::getCenter() const
{
	return (this->maxPoint+this->minPoint)/2.0f;
}
