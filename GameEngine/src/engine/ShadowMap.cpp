#include "ShadowMap.h"

ShadowMap::ShadowMap(std::shared_ptr<unsigned int> screenWidth, std::shared_ptr<unsigned int> screenHeight)
	:fboId(0), texId(0),
	debugFrameBuffer("../GameEngine/resources/shader/ShadowMap/debugShadowMap_vertex.shader", "../GameEngine/resources/shader/ShadowMap/debugShadowMap_fragment.shader"),
	lightSpaceMatrix(1.0f)
{
	pScreenWidth = screenWidth;
	pScreenHeight = screenHeight;

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::OnUpdate(const glm::vec3& lightPosition, const glm::vec3& lightDirection, float fov, const std::vector<Model>& objects)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glClear(GL_DEPTH_BUFFER_BIT);

	auto& shader = ShaderManager::GetShader(SHADER_TYPE::SHADOW_SPOT);
	shader.Bind();
	
	auto lightPosAtHead = lightPosition + glm::vec3(0.0f, 1.2f, 0.0f);
	float near_plane = 1.0f, far_plane = 17.5f;

	glm::mat4 lightProjection = glm::perspective(0.9f, 1.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPosAtHead, lightPosAtHead + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	shader.SetUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

	for (const auto& obj : objects)
	{
		obj.Draw(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 2);
	glViewport(0, 0, *pScreenWidth, *pScreenHeight);
}

void ShadowMap::Bind(const Shader& shader, unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texId);

	shader.SetUniform1i("shadowMap", slot);
}

void ShadowMap::DebugShadowMap() const
{
	debugFrameBuffer.OnRenderTexture(texId);
}
