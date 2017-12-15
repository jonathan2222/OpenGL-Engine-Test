#include "Camera.h"

#include "glm\gtc\matrix_transform.hpp"
#include <cmath>

#include "../Constants.h"

Camera::Camera(const Display * display)
{
	this->position = glm::vec3(0.0f, 0.0f, 1.0f);
	this->display = display;
	init();
	createProj();
	lookAt({0.0f, 0.0f, 0.0f});
}

Camera::Camera(const Display * display, const glm::vec3& position, const glm::vec3& target)
{
	this->position = position;
	this->display = display;
	init();
	createProj();
	lookAt(target);
}

void Camera::init()
{
	this->yawPitchRoll = glm::vec3(0.0f, 0.0f, 0.0f);
	this->fov = glm::pi<float>() / 4.0f;
	glfwSetCursorPos(this->display->getWindowPtr(), this->display->getWidth() / 2, this->display->getHeight() / 2);
	glfwSetInputMode(this->display->getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::updateProjection(const Display * display)
{
	glfwSetCursorPos(this->display->getWindowPtr(), this->display->getWidth() / 2, this->display->getHeight() / 2);
	this->display = display;
	createProj();
	this->pv = this->proj*this->view;
}

void Camera::update(GLuint shaderProgramID, float dt)
{
	double xPos, yPos;
	glfwGetCursorPos(this->display->getWindowPtr(), &xPos, &yPos);
	xPos -= this->display->getWidth() / 2;
	yPos -= this->display->getHeight() / 2;
	if (xPos != 0.0 || yPos != 0.0)
	{
		this->yawPitchRoll.x += xPos*dt*0.5f;
		this->yawPitchRoll.y -= yPos*dt*0.5f;
		
		rotate(this->yawPitchRoll.x, this->yawPitchRoll.y, this->yawPitchRoll.z);
	}

	glfwSetCursorPos(this->display->getWindowPtr(), this->display->getWidth() / 2, this->display->getHeight() / 2);

	if (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_W) == GLFW_PRESS)
		translate(this->f*CAMERA_SPEED);
	if (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_S) == GLFW_PRESS)
		translate(-this->f*CAMERA_SPEED);
	if (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_D) == GLFW_PRESS)
		translate(this->r*CAMERA_SPEED);
	if (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_A) == GLFW_PRESS)
		translate(-this->r*CAMERA_SPEED);

	applyCamera(shaderProgramID);
}

void Camera::rotate(float yaw, float pitch, float roll)
{
	this->f.x = cos(yaw-3.1415f/2.0f)*cos(pitch);
	this->f.y = sin(pitch);
	this->f.z = sin(yaw - 3.1415f / 2.0f)*cos(pitch);
	this->r = glm::normalize(glm::cross(f, GLOBAL_UP_VECTOR));
	this->u = glm::cross(r, f);
	this->yawPitchRoll.x = yaw;
	this->yawPitchRoll.y = pitch;
	this->yawPitchRoll.z = roll;

	this->view = glm::mat4({ r.x, u.x, -f.x, 0.0f },
	{ r.y, u.y, -f.y, 0.0f },
	{ r.z, u.z, -f.z, 0.0f },
	{ -glm::dot(r, this->position), -glm::dot(u, this->position), -glm::dot(-f, this->position), 1.0f });

	this->pv = this->proj*this->view;
}

void Camera::translate(const glm::vec3 & offset)
{
	this->position += offset;
	this->view = glm::mat4({ r.x, u.x, -f.x, 0.0f },
	{ r.y, u.y, -f.y, 0.0f },
	{ r.z, u.z, -f.z, 0.0f },
	{ -glm::dot(r, this->position), -glm::dot(u, this->position), -glm::dot(-f, this->position), 1.0f });

	this->pv = this->proj*this->view;
}

void Camera::lookAt(const glm::vec3 & target)
{
	this->f = glm::normalize(target- this->position);
	this->r = glm::normalize(glm::cross(f, GLOBAL_UP_VECTOR));
	this->u = glm::cross(r, f);

	this->yawPitchRoll.x = atan2(this->f.z, this->f.x)+3.1415f/2.0f;
	this->yawPitchRoll.y = atan2(this->f.y, -this->f.z);
	this->yawPitchRoll.z = atan2(this->u.y, this->f.x);

	this->view = glm::mat4({ r.x, u.x, -f.x, 0.0f },
	{ r.y, u.y, -f.y, 0.0f },
	{ r.z, u.z, -f.z, 0.0f },
	{ -glm::dot(r, this->position), -glm::dot(u, this->position), -glm::dot(-f, this->position), 1.0f });
	
	this->pv = this->proj*this->view;
}

void Camera::lookAt(const Mesh & mesh)
{
	lookAt(mesh.getCenter());
}

void Camera::moveAndLookAt(const Mesh & mesh)
{
	float camDepth = (mesh.getWidth() + mesh.getHeight() + mesh.getDepth());
	this->position = glm::vec3(0.0f, mesh.getHeight() / 2.0f, camDepth);
	glm::vec3 camTarget = mesh.getCenter();
	lookAt(camTarget);
}

void Camera::setFOV(float fov)
{
	this->fov = fov;
}

void Camera::setPosition(const glm::vec3& postion)
{
	this->translate(position-this->position);
}

glm::vec3 Camera::getDirection() const
{
	return this->f;
}

glm::vec3 Camera::getPosition() const
{
	return this->position;
}

glm::mat4 Camera::getViewMatrix() const
{
	return this->view;
}

glm::mat4 Camera::getProjMatrix() const
{
	return this->proj;
}

glm::mat4 Camera::getMatrix() const
{
	return this->pv;
}

glm::vec3 Camera::getYawPitchRoll() const
{
	return this->yawPitchRoll;
}

void Camera::applyCamera(GLuint shaderProgramID)
{
	GLuint pv = glGetUniformLocation(shaderProgramID, "camera");
	if (pv == -1) fprintf(stderr, "Can't find 'camera' in the shader!");
	glUniformMatrix4fv(pv, 1, GL_FALSE, &this->pv[0][0]);

	//GLuint camPos = glGetUniformLocation(shaderProgramID, "camPos");
	//if (camPos == -1) fprintf(stderr, "Can't find 'camPos' in the shader!");
	//glUniformMatrix4fv(camPos, 1, GL_FALSE, &this->position[0]);
}

void Camera::createProj()
{
	this->proj = glm::perspective(this->fov, this->display->getRatio(), NEAR_PLANE, FAR_PLANE);
}
