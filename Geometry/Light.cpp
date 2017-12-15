#include "Light.h"

Light::Light()
{
	init({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.8f, 10.0f);
}

Light::Light(const glm::vec3 & pos, const glm::vec3 & color, float intensity, float radius)
{
	init(pos, color, intensity, radius);
}

void Light::init(const glm::vec3 & pos, const glm::vec3 & color, float intensity, float radius)
{
	this->position = pos;
	this->color = color;
	this->intensity = intensity;
	this->radius = radius;
	rot = glm::mat4(1.0f);
}

void Light::rotate(float angle, const glm::vec3 & axis)
{
	rot = glm::rotate(rot, angle, axis);
}

void Light::setPosition(const glm::vec3 & pos)
{
	this->position = pos;
}

glm::vec3 Light::getPosition() const
{
	glm::vec4 r = this->rot*glm::vec4(this->position, 1.0f);
	return glm::vec3(r.x, r.y, r.z);
}

glm::vec3 Light::getColor() const
{
	return this->color;
}

float Light::getIntensity() const
{
	return this->intensity;
}

float Light::getRadius() const
{
	return this->radius;
}
