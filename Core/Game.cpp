#include "Game.h"

#include "../Shade/ShaderProgram.h"
#include "../Shade/Shader.h"

#include "..\Geometry\Mesh.h"
#include "..\Loader\ObjLoader.h"

#include "../Geometry/Light.h"

#include "../Renderer/DeferredRenderer.h"

#include <iostream>

#include <chrono>

Game::Game()
{
	this->display = new Display("OpenGL Testing");

	this->camera = new Camera(this->display);

	GLint glMajor, glMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glMinor);
	std::cout << "OpenGL context version " << glMajor << "." << glMinor << " created." << std::endl;

	gameLoop();
}

Game::~Game()
{
	delete this->display;
	delete this->camera;
}

void Game::gameLoop()
{
	DeferredRenderer dr(this->display);

	Mesh mesh;
	ObjLoader loader;
	//loader.load(&mesh, "Bunny/bunny.obj");
	loader.load(&mesh, "Buddha/buddha2.obj");
	//loader.load(&mesh, "Cube/Cube.obj");
	mesh.create(dr.getGeometryShaderID());
	mesh.createBoundingBox();

	dr.bindAsGeometryRenderTarget();

	this->camera->moveAndLookAt(mesh);
	glUseProgram(dr.getGeometryShaderID());
	this->camera->applyCamera(dr.getGeometryShaderID());

	dr.unbindGeometryRenderTarget();

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto preTime = currentTime;
	float dt = 0.0f;

	glfwSetInputMode(this->display->getWindowPtr(), GLFW_STICKY_KEYS, GL_TRUE);
	while (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(this->display->getWindowPtr()) == 0)
	{

		if (this->display->shouldUpdateSize)
		{
			dr.bindAsGeometryRenderTarget();

			this->camera->updateProjection(this->display);
			glUseProgram(dr.getGeometryShaderID());
			this->camera->applyCamera(dr.getGeometryShaderID());

			dr.unbindGeometryRenderTarget();

			this->display->shouldUpdateSize = false;
		}


		glUseProgram(dr.getGeometryShaderID());
		this->camera->update(dr.getGeometryShaderID(), dt);

		// Draw
		dr.bindAsGeometryRenderTarget();

		glUseProgram(dr.getGeometryShaderID());
		mesh.rotate(0.01f, { 0.0f, 1.0f, 0.0f });
		mesh.draw();

		dr.unbindGeometryRenderTarget();

		dr.drawToScreen();

		// Swap buffers
		glfwSwapBuffers(this->display->getWindowPtr());
		glfwPollEvents();

		currentTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float>(currentTime - preTime).count();
		preTime = currentTime;
	}

	/*
	Shader vert("Phong.vs", GL_VERTEX_SHADER);
	Shader frag("Phong.fs", GL_FRAGMENT_SHADER);
	ShaderProgram sp(vert, frag);

	Mesh mesh;
	ObjLoader loader;
	loader.load(&mesh, "Bunny/bunny.obj");
	//loader.load(&mesh, "Buddha/buddha2.obj");
	//loader.load(&mesh, "Cube/Cube.obj");
	mesh.create(sp.getID());
	mesh.createBoundingBox();

	this->camera->moveAndLookAt(mesh);
	glUseProgram(sp.getID());
	this->camera->applyCamera(sp.getID());

	Light lights[5];
	lights[0] = Light(this->camera->getPosition() + glm::vec3(0.0f, 2.0f, 0.0f), { 1.0f, 1.0f, 1.0f }, 0.8f, 10.0f);
	lights[1] = Light(this->camera->getPosition() + glm::vec3(4.0f, 2.0f, 2.0f), { 1.0f, 0.0f, 0.0f }, 0.2f, 10.0f);
	lights[2] = Light(this->camera->getPosition() + glm::vec3(-4.0f, 1.5f, 1.0f), { 0.0f, 1.0f, 0.0f }, 0.2f, 10.0f);
	lights[3] = Light(this->camera->getPosition() + glm::vec3(2.0f, 2.5f, 5.0f), { 0.0f, 0.0f, 1.0f }, 0.2f, 10.0f);
	lights[4] = Light(this->camera->getPosition() + glm::vec3(-2.0f, 2.0f, 4.0f), { 1.0f, 0.0f, 1.0f }, 0.2f, 10.0f);

	glUseProgram(sp.getID());

	GLuint bindingPoint = 2;
	GLuint uboIndex = glGetUniformBlockIndex(sp.getID(), "Lights");
	glUniformBlockBinding(sp.getID(), uboIndex, bindingPoint);

	glUseProgram(0);

	GLuint UBOLights;
	glGenBuffers(1, &UBOLights);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOLights);

	struct LightsData
	{
		struct LightData
		{
			float positionRadius[4];
			float colorIntensity[4];
			float specularColor[4];
		} lightData[10];
		int numLights;
		int padding[3];
	} lightsData;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
			lightsData.lightData[i].positionRadius[j] = lights[i].getPosition()[j];
		lightsData.lightData[i].positionRadius[3] = lights[i].getRadius();

		for (int j = 0; j < 3; j++)
			lightsData.lightData[i].colorIntensity[j] = lights[i].getColor()[j];
		lightsData.lightData[i].colorIntensity[3] = lights[i].getIntensity();

		for (int j = 0; j < 3; j++)
			lightsData.lightData[i].specularColor[j] = lights[i].getColor()[j];
	}
	lightsData.numLights = 5;
	fprintf(stderr, "Pos: (%f, %f, %f) \n", lights[0].getPosition().x, lights[0].getPosition().y, lights[0].getPosition().z);
	fprintf(stderr, "Pos: (%f, %f, %f) \n", lightsData.lightData[0].positionRadius[0], lightsData.lightData[0].positionRadius[1], lightsData.lightData[0].positionRadius[2]);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsData), &lightsData, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBOLights);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto preTime = currentTime;
	float dt = 0.0f;

	glfwSetInputMode(this->display->getWindowPtr(), GLFW_STICKY_KEYS, GL_TRUE);
	while (glfwGetKey(this->display->getWindowPtr(), GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(this->display->getWindowPtr()) == 0)
	{
		if (this->display->shouldUpdateSize)
		{
			this->camera->updateProjection(this->display);
			glUseProgram(sp.getID());
			this->camera->applyCamera(sp.getID());

			this->display->shouldUpdateSize = false;
		}

		this->camera->update(sp.getID(), dt);

		// Draw
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(sp.getID());
		
		for (int i = 0; i < 5; i++)
		{
			lights[i].rotate(dt, {0.0f, 1.0f, 0.0f});
			for (int j = 0; j < 3; j++)
				lightsData.lightData[i].positionRadius[j] = lights[i].getPosition()[j];
		}
		glBindBuffer(GL_UNIFORM_BUFFER, UBOLights);
		void* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
		memcpy(data, &lightsData, sizeof(LightsData));
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		//mesh.rotate(0.01f, { 0.0f, 1.0f, 0.0f });
		mesh.draw();

		// Swap buffers
		glfwSwapBuffers(this->display->getWindowPtr());
		glfwPollEvents();

		currentTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float>(currentTime - preTime).count();
		preTime = currentTime;
	}
	*/
}
