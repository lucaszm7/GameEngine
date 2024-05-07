#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>

#include "model.h"
#include "ShaderManager.h"
#include "FrameBuffer.hpp"

class ShadowMap
{
	unsigned int fboId;
	unsigned int texId;

	FrameBuffer debugFrameBuffer;

	inline static const unsigned int SHADOW_WIDTH = 1024;
	inline static const unsigned int SHADOW_HEIGHT = 1024;

	inline static std::shared_ptr<unsigned int> pScreenWidth;
	inline static std::shared_ptr<unsigned int> pScreenHeight;

public:
	glm::mat4 lightSpaceMatrix;

	ShadowMap(std::shared_ptr<unsigned int> screenWidth, std::shared_ptr<unsigned int> screenHeight);
	void OnUpdate(const glm::vec3& lightPosition, const glm::vec3& lightDirection, float fov, const std::vector<Model>& objects);
	void Bind(const Shader& shader, unsigned int slot);
	void DebugShadowMap() const;
};

