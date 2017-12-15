#ifndef LIGHT_H
#define LIGHT_H

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

class Light
{
public:
	Light();
	Light(const glm::vec3& pos, const glm::vec3& color, float intensity, float radius);

	void init(const glm::vec3& pos, const glm::vec3& color, float intensity, float radius);

	void rotate(float angle, const glm::vec3& axis);

	void setPosition(const glm::vec3& pos);
	glm::vec3 getPosition() const;
	glm::vec3 getColor() const;
	float getIntensity() const;
	float getRadius() const;

private:
	glm::mat4 rot;
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float radius;
};

#endif