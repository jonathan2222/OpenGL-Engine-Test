#include "DeferredRenderer.h"

const GLfloat DeferredRenderer::quadVertexBufferData[30] = {
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f
};

DeferredRenderer::DeferredRenderer(Display* display)
{
	this->display = display;
	init();
}

DeferredRenderer::~DeferredRenderer()
{
	delete this->geometryShader;
	delete this->geometryVS;
	delete this->geometryFS;
	delete this->finalShader;
	delete this->finalVS;
	delete this->finalFS;

	glDeleteVertexArrays(1, &this->quadVAO);
	glDeleteBuffers(1, &this->quadVBO);
	delete gbuffer;
}

void DeferredRenderer::bindAsGeometryRenderTarget()
{
	this->gbuffer->bindAsRenderTarget();
}

void DeferredRenderer::unbindGeometryRenderTarget()
{
	this->gbuffer->unbindRenderTarget();
}

void DeferredRenderer::drawToScreen()
{
	/*
	glViewport(0, 0, this->display->getWidth(), this->display->getHeight());
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer);

	
	GLsizei halfWidth = (GLsizei)(this->display->getWidth()/2);
	GLsizei halfHeight = (GLsizei)(this->display->getHeight() / 2);
	GLsizei width = (GLsizei)this->display->getWidth();
	GLsizei height = (GLsizei)this->display->getHeight();

	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, width, height, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, width, height, halfWidth, 0, width, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, width, height, 0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	*/
	
	glViewport(0, 0, this->display->getWidth(), this->display->getHeight());

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(this->finalShader->getID());

	glUniform1i(glGetUniformLocation(this->finalShader->getID(), "diffuseTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gbuffer->getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE));

	glUniform1i(glGetUniformLocation(this->finalShader->getID(), "normalTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->gbuffer->getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL));

	glUniform1i(glGetUniformLocation(this->finalShader->getID(), "positionTexture"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->gbuffer->getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION));

	glUniform1i(glGetUniformLocation(this->finalShader->getID(), "depthTexture"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, this->gbuffer->getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_DEPTH));

	glBindVertexArray(this->quadVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}

GLuint DeferredRenderer::getGeometryShaderID() const
{
	return this->geometryShader->getID();
}

void DeferredRenderer::init()
{
	this->gbuffer = new GBuffer(this->display->getWidth(), this->display->getHeight());

	// Prepare data for quad
	glGenVertexArrays(1, &this->quadVAO);
	glBindVertexArray(this->quadVAO);
	glGenBuffers(1, &this->quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->quadVertexBufferData), this->quadVertexBufferData, GL_STATIC_DRAW);

	this->geometryVS = new Shader("GeometryPassDR.vs", GL_VERTEX_SHADER);
	this->geometryFS = new Shader("GeometryPassDR.fs", GL_FRAGMENT_SHADER);
	this->geometryShader = new ShaderProgram(*this->geometryVS, *this->geometryFS);

	this->finalVS = new Shader("PassthroughDR.vs", GL_VERTEX_SHADER);
	this->finalFS = new Shader("PassthroughDR.fs", GL_FRAGMENT_SHADER);
	this->finalShader = new ShaderProgram(*this->finalVS, *this->finalFS);

	glUseProgram(this->finalShader->getID());

	GLuint posID = glGetAttribLocation(this->finalShader->getID(), "vertexPosition");
	if (posID == -1) { fprintf(stderr, "Error, cannot find 'vertexPosition' attribute in PassthroughDR.vs! \n"); return; }
	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(nullptr));
	
	GLuint uvsID = glGetAttribLocation(this->finalShader->getID(), "vertexUvs");
	if (uvsID == -1) { fprintf(stderr, "Error, cannot find 'vertexUvs' attribute in PassthroughDR.vs! \n"); return; }
	glEnableVertexAttribArray(uvsID);
	glVertexAttribPointer(uvsID, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(sizeof(GL_FLOAT)*3));

	glBindVertexArray(0);
}
/*
void DeferredRenderer::createTexturesAndDepthbuffer()
{
	// Textures
	this->textures = new GLuint[GBUFFER_NUM_TEXTURES];
	glGenTextures(GBUFFER_NUM_TEXTURES, this->textures);
	for (int i = 0; i < GBUFFER_NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->display->getWidth(), this->display->getHeight(), 0, GL_RGB, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0); // Attach a color attachment to the texture.
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Depth buffer
	glGenRenderbuffers(1, &this->depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->display->getWidth(), this->display->getHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthRenderbuffer);
}*/
