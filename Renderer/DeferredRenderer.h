#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "GL\glew.h"
#include "../Core/Display.h"

#include "../Shade/ShaderProgram.h"
#include "../Shade/Shader.h"

#include "../Geometry/Mesh.h"
#include "GBuffer.h"

class DeferredRenderer
{
public:
	DeferredRenderer(Display* display);
	~DeferredRenderer();

	void bindAsGeometryRenderTarget();

	void unbindGeometryRenderTarget();

	void drawToScreen();

	GLuint getGeometryShaderID() const;

private:
	void init();

private:
	GBuffer* gbuffer;

	Display* display;

	GLuint quadVAO;
	GLuint quadVBO;

	static const GLfloat quadVertexBufferData[30];
	ShaderProgram* finalShader = nullptr;
	Shader* finalVS = nullptr;
	Shader* finalFS = nullptr;

	ShaderProgram* geometryShader = nullptr;
	Shader* geometryVS = nullptr;
	Shader* geometryFS = nullptr;
};

#endif
