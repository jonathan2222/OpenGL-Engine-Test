#ifndef GBUFFER_H
#define GBUFFER_H

#include "GL\glew.h"

class GBuffer
{
public:
	GBuffer(int width, int height);
	~GBuffer();

	void resize(int width, int height);

	void bindAsRenderTarget();
	void unbindRenderTarget();

	int getWidth() const;
	int getHeight() const;

	enum GBUFFER_TEXTURE_TYPE
	{
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DEPTH,
		GBUFFER_NUM_TEXTURES
	};

	GLuint getTexture(GBUFFER_TEXTURE_TYPE type) const;

private:
	void init();
	void createTexture(GLuint &id, bool isDepth);

	GLuint gBuffer;
	GLuint* textures;

	int width;
	int height;
};

#endif
