#pragma once

#include <GL/glew.h>

class ShadowMap
{
	unsigned int fboId;
	unsigned int texId;

	inline static const unsigned int SHADOW_WIDTH = 1024;
	inline static const unsigned int SHADOW_HEIGHT = 1024;

	ShadowMap();
	void OnUpdate();
};

