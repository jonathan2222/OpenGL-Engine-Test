#ifndef CAMERA_H
#define CAMERA_H

#include "glm\glm.hpp"
#include "../Core/Display.h"
#include "../Geometry/Mesh.h"

class Camera
{
public:
	Camera(const Display* display);
	Camera(const Display * display, const glm::vec3& position, const glm::vec3& target);

	void init();

	void updateProjection(const Display * display);

	void update(GLuint shaderProgramID, float dt);

	void rotate(float yaw, float pitch, float roll);
	void translate(const glm::vec3& offset);

	void lookAt(const glm::vec3& target);
	void lookAt(const Mesh& mesh);
	void moveAndLookAt(const Mesh& mesh);

	void setFOV(float fov);
	void setPosition(const glm::vec3& postion);

	glm::vec3 getDirection() const;
	glm::vec3 getPosition() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjMatrix() const;
	glm::mat4 getMatrix() const;
	glm::vec3 getYawPitchRoll() const;

	void applyCamera(GLuint shaderProgramID);

private:
	void createProj();

private:
	float fov;
	glm::vec3 position;
	glm::vec3 yawPitchRoll;

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 pv;

	glm::vec3 f;
	glm::vec3 r;
	glm::vec3 u;

	const Display* display;
};

#endif
