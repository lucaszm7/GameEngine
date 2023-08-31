#include "ShadowMap.h"

ShadowMap::ShadowMap(std::shared_ptr<unsigned int> screenWidth, std::shared_ptr<unsigned int> screenHeight)
	:fboId(0), texId(0),
	shader  ("../resources/shader/ShadowMap/shadowMap_vertex.shader",      "../resources/shader/ShadowMap/shadowMap_fragment.shader")
{
	pScreenWidth = screenWidth;
	pScreenHeight = screenHeight;

	glGenFramebuffers(1, &fboId);

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::OnUpdate(const glm::vec3& lightPosition, const std::vector<Model>& objects)
{

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glClear(GL_DEPTH_BUFFER_BIT);
	shader.Bind();
	
	float near_plane = 1.0f, far_plane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	shader.SetUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

	for (const auto& obj : objects)
	{
		obj.Draw(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 2);
	glViewport(0, 0, *pScreenWidth, *pScreenHeight);
}

void ShadowMap::Bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texId);
}

void ShadowMap::DebugShadowMap() const
{
}
