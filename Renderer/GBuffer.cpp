#include "GBuffer.h"

GBuffer::GBuffer(int width, int height)
{
	this->width = width;
	this->height = height;

	init();
}

GBuffer::~GBuffer()
{
	glDeleteFramebuffers(1, &this->gBuffer);
	glDeleteTextures(GBUFFER_NUM_TEXTURES, this->textures);
	delete[] this->textures;
}

GLuint GBuffer::getTexture(GBUFFER_TEXTURE_TYPE type) const
{
	return this->textures[type];
}

void GBuffer::init()
{
	// Textures
	this->textures = new GLuint[GBUFFER_NUM_TEXTURES];
	glGenTextures(GBUFFER_NUM_TEXTURES, this->textures);
	createTexture(textures[GBUFFER_TEXTURE_TYPE_DIFFUSE], false);
	createTexture(textures[GBUFFER_TEXTURE_TYPE_NORMAL], false);
	createTexture(textures[GBUFFER_TEXTURE_TYPE_POSITION], false);
	createTexture(textures[GBUFFER_TEXTURE_TYPE_DEPTH], true);

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);
	GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_NONE };

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textures[GBUFFER_TEXTURE_TYPE_DIFFUSE], 0); // Attach a color attachment to the texture.
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->textures[GBUFFER_TEXTURE_TYPE_NORMAL], 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->textures[GBUFFER_TEXTURE_TYPE_POSITION], 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->textures[GBUFFER_TEXTURE_TYPE_DEPTH], 0);
	glDrawBuffers(4, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GBuffer::createTexture(GLuint & id, bool isDepth)
{
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, 
		isDepth ? GL_DEPTH_COMPONENT24 : GL_RGB32F, 
		getWidth(), getHeight(), 0, 
		isDepth ? GL_DEPTH_COMPONENT : GL_RGB, 
		GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GBuffer::resize(int width, int height)
{
	this->width = width;
	this->height = height;

	// TODO: Make this!
}

void GBuffer::bindAsRenderTarget()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->gBuffer);
	glViewport(0, 0, getWidth(), getHeight());

	glDepthMask(GL_TRUE); // Enable writing to depth buffer
	glEnable(GL_DEPTH_TEST); // Enable test for depth when rendering
	glDisable(GL_BLEND);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::unbindRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Restore default fbo
	glDepthMask(GL_FALSE); // Disable writing for depth buffer
	glDisable(GL_DEPTH_TEST); // Disable test for depth when rendering
}

int GBuffer::getWidth() const
{
	return this->width;
}

int GBuffer::getHeight() const
{
	return this->height;
}
