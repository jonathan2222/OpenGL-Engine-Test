#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm\glm.hpp"

class Material
{
public:
	Material()
	{
		this->ka = { 0.2f, 0.2f, 0.2f };
		this->kd = { 0.6f, 0.6f, 0.6f };
		this->ks = { 1.0f, 1.0f, 1.0f };
		this->ns = 90.0f;
	}

	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float ns;

	int width;
	int height;
	unsigned char* imgData = nullptr;

	~Material() { delete imgData; };

};

#endif
