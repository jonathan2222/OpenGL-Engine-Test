#ifndef MODEL_H
#define MODEL_H

#include "GL\glew.h"
#include "glm\glm.hpp"
#include <vector>

#include "Material.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void create(const GLuint shaderProgramID);
	void draw() const;

	void rotate(float angle, const glm::vec3& axis);

	void createBoundingBox();
	float getWidth() const;
	float getHeight() const;
	float getDepth() const;
	glm::vec3 getCenter() const;

public:
	struct Vertex
	{
		float position[3];
		float normal[3];
		float uvs[2];
	};

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	Material* material = nullptr;

private:
	glm::mat4 model;
	glm::mat4 world;
	GLuint modelMat;
	GLuint worldMat;

	GLuint vertexBufferID = 0;
	GLuint vaoID = 0;
	GLuint indexBufferID = 0;

	glm::vec3 maxPoint;
	glm::vec3 minPoint;

	GLuint UBOMaterial;
	struct MaterialData
	{
		float ka[4];
		float kd[4];
		float ks[4];
	} materialData;
};

#endif
